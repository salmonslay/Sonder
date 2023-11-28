// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotHookingState.h"

#include "CharacterStateMachine.h"
#include "RobotBaseState.h"
#include "RobotStateMachine.h"
#include "SoulCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HookExplosionActor.h"
#include "HookShotAttachment.h"
#include "StaticsHelper.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

URobotHookingState::URobotHookingState()
{
	SetIsReplicatedByDefault(true); 
}

void URobotHookingState::Enter()
{
	Super::Enter();

	if(!SoulCharacter)
		SoulCharacter = UGameplayStatics::GetActorOfClass(this, ASoulCharacter::StaticClass()); 
	
	if(!RobotCharacter)
		RobotCharacter = Cast<ARobotStateMachine>(PlayerOwner);

	if(!MovementComponent)
		MovementComponent = RobotCharacter->GetCharacterMovement();

	DefaultGravityScale = MovementComponent->GravityScale;

	FailSafeTimer = 0;

	StartLocation = PlayerOwner->GetActorLocation();

	CurrentTargetActor = nullptr; 

	SetHookTarget(); 
	
	StartShootHook(); 
}

void URobotHookingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	// Fail safe to ensure player does not get stuck while using hook shot. Can prob be removed 
	if((FailSafeTimer += DeltaTime) >= FailSafeLength)
		EndHookShot();

	// Shooting the hook towards its target, does not need to do anything else 
	if(bShootingHookOutwards)
	{
		ShootHook(DeltaTime);
		return; 
	}

	if(bTravellingTowardsTarget)
	{
		FHitResult HitResult; 
		GetActorToTarget(HitResult);
	
		// Perform line trace while travelling towards target to see if something is now blocking (for whatever reason)
		if(HitResult.IsValidBlockingHit())
		{
			EndHookShot();
			return; 
		}
		
		TravelTowardsTarget(DeltaTime);
	} else // Hit an object, retract hook 
		RetractHook(DeltaTime); 
}

void URobotHookingState::Exit()
{
	Super::Exit();

	// UE_LOG(LogTemp, Warning, TEXT("Exited hook state"))

	if(!RobotCharacter->InputEnabled())
		RobotCharacter->EnableInput(RobotCharacter->GetLocalViewingPlayerController());

	PlayerOwner->SetCanBeDamaged(true);

	PlayerOwner->GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &URobotHookingState::ActorOverlap);

	// Calculate new velocity, defaulting at current velocity 
	const auto MovementComp = PlayerOwner->GetCharacterMovement();
	FVector NewVel = MovementComp->Velocity; 
	
	// If targeted a static hook or Soul 
	if(bTravellingTowardsTarget) // Set velocity to zero if Soul otherwise keep some momentum 
		NewVel = bHookTargetIsSoul ? FVector::ZeroVector : MovementComp->Velocity / VelocityDivOnReachedHook;
	
	ServerRPCHookShotEnd(RobotCharacter, NewVel);
	
	bTravellingTowardsTarget = false;
	bHookShotActive = false; 
}

void URobotHookingState::EndHookShot(const bool bEndFromDash) const
{
	// Dont end if called from dash and target is NOT Soul 
	if(bEndFromDash && !bHookTargetIsSoul)
		return; 
	
	// Change state if this state is active 
	if(Cast<ARobotStateMachine>(GetOwner())->GetCurrentState() == this)
		PlayerOwner->SwitchState(RobotCharacter->RobotBaseState);
}

void URobotHookingState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URobotHookingState, CurrentHookTargetLocation)
	DOREPLIFETIME(URobotHookingState, bHookShotActive)
	DOREPLIFETIME(URobotHookingState, HookArmLocation)
}

bool URobotHookingState::SetHookTarget()
{
	FHitResult HitResult;
	CurrentTargetActor = GetActorToTarget(HitResult);

	if(!SoulCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No soul in SetHookTarget"))
		CurrentHookTargetLocation = PlayerOwner->GetActorLocation(); 
		return false;
	}
	
	CurrentHookTargetLocation = CurrentTargetActor ? CurrentTargetActor->GetActorLocation() : HitResult.Location;

	// Offset target location if targeting Soul 
	if(CurrentTargetActor == SoulCharacter)
	{
		CurrentHookTargetLocation += FVector::UpVector * ZSoulTargetOffset;
		bHookTargetIsSoul = true; 
	} else
		bHookTargetIsSoul = false; 

	if(!CurrentTargetActor || !StaticsHelper::ActorIsInFront(RobotCharacter, CurrentHookTargetLocation))
	{
		CurrentHookTargetLocation = GetTargetOnNothingInFront();
		ServerRPC_SetHookTarget(CurrentHookTargetLocation); 
		return false; 
	} 

	ServerRPC_SetHookTarget(CurrentHookTargetLocation); 
	return !HitResult.IsValidBlockingHit(); 
}

void URobotHookingState::ServerRPC_SetHookTarget_Implementation(const FVector& NewTarget)
{
	CurrentHookTargetLocation = NewTarget; 
}

FVector URobotHookingState::GetTargetOnNothingInFront() const
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerOwner);

	FVector EndLoc = PlayerOwner->GetActorLocation() + PlayerOwner->GetActorForwardVector() * MaxHookShotDistanceOnBlock; 
	
	GetWorld()->LineTraceSingleByChannel(HitResult, PlayerOwner->GetActorLocation(), EndLoc, ECC_Pawn, Params);

	// Return hit location or end loc if no hit 
	return HitResult.IsValidBlockingHit() ? HitResult.Location : EndLoc; 
}

AActor* URobotHookingState::GetActorToTarget(FHitResult& HitResultOut)
{
	if(!SoulCharacter)
		SoulCharacter = UGameplayStatics::GetActorOfClass(this, ASoulCharacter::StaticClass()); 

	if(!SoulCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No soul when attempting line trace"))
		return nullptr; 
	}

	const FVector StartLoc = PlayerOwner->GetActorLocation();
	FVector EndLoc = CurrentHookTargetLocation;

	AHookShotAttachment* HookTarget = nullptr;
	bool bTargetingSoul = true; 

	// Set new end loc if player does not already have a target 
	if(!bTravellingTowardsTarget)
	{
		EndLoc = SoulCharacter->GetActorLocation(); // Default target is Soul

		// If Soul is NOT in front of Robot, only then check if there is a possible hook point to target 
		if(!StaticsHelper::ActorIsInFront(RobotCharacter, EndLoc))
		{
			// Set EndLoc to Hook location of there is an eligible hook target 
			HookTarget = AHookShotAttachment::GetHookToTarget(RobotCharacter); 
			if(HookTarget)
			{
				EndLoc = HookTarget->GetActorLocation();
				bTargetingSoul = false; 
			}
		}
	} else if(bHookTargetIsSoul) // Update target loc if target is Soul, Soul could've moved 
		CurrentHookTargetLocation = SoulCharacter->GetActorLocation(); 

	FCollisionQueryParams Params;
	Params.AddIgnoredActors( TArray<AActor*>( {PlayerOwner, SoulCharacter, HookTarget } ) ); // Ignore players and hook 

	const auto CapsuleComp = PlayerOwner->GetCapsuleComponent();
	
	// Sweep instead of Line Trace 
	const FCollisionShape CollShape = FCollisionShape::MakeCapsule(CapsuleComp->GetScaledCapsuleRadius() / 2, CapsuleComp->GetScaledCapsuleHalfHeight() / 2);
	GetWorld()->SweepSingleByChannel(HitResultOut, StartLoc, EndLoc, FQuat::Identity, ECC_Pawn, CollShape, Params); 

	// Return the Hook or Soul if sweep trace did not impact an obstacle 
	if(!HitResultOut.IsValidBlockingHit())
		return HookTarget ? HookTarget : SoulCharacter;

	// Hit an obstacle and is targeting Soul
	if(bTargetingSoul)
	{
		// Get eventual Hook
		HookTarget = AHookShotAttachment::GetHookToTarget(RobotCharacter);
		if(HookTarget) // If there is a valid HookTarget, update the hit result with line trace towards hook instead of towards Soul 
			GetWorld()->SweepSingleByChannel(HitResultOut, StartLoc, HookTarget->GetActorLocation(), FQuat::Identity, ECC_Pawn, CollShape, Params);

		// Return hook target if there is a clear path to it, otherwise return nullptr - no valid target 
		return HitResultOut.IsValidBlockingHit() ? nullptr : HookTarget; 
	}

	// No valid target 
	return nullptr; 
}

// Run once when hook shoots out 
void URobotHookingState::StartShootHook()
{
	// Disable input if there is a valid target 
	if(CurrentTargetActor)
		RobotCharacter->DisableInput(RobotCharacter->GetLocalViewingPlayerController());
	
	bShootingHookOutwards = true;
	bHookShotActive = true; 
	
	ServerRPCHookShotStart(CurrentHookTargetLocation, RobotCharacter); 
}

void URobotHookingState::ServerRPCHookShotStart_Implementation(const FVector& HookTarget, ARobotStateMachine* RobotChar)
{
	if(!PlayerOwner->HasAuthority())
		return;

	HookArmLocation = PlayerOwner->GetActorLocation(); 

	bHookShotActive = true; 

	MulticastRPCHookShotStart(HookTarget, RobotChar); 
}

void URobotHookingState::MulticastRPCHookShotStart_Implementation(const FVector& HookTarget, ARobotStateMachine* RobotChar)
{
	if(!GetOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null"))
		return; 
	}
	
	if(CurrentTargetActor) // If there is a valid target 
	{
		PlayerOwner->GetCharacterMovement()->GravityScale = 0;
		PlayerOwner->GetCharacterMovement()->Velocity = FVector::ZeroVector; 
		PlayerOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying); 
	}

	RobotChar->OnHookShotStart(); 
}

// Run each Tick 
void URobotHookingState::ShootHook(const float DeltaTime) 
{
	// Shoots the hook outwards

	// Lerp the hook arm towards the target 
	HookArmLocation = UKismetMathLibrary::VInterpTo_Constant(HookArmLocation, CurrentHookTargetLocation, DeltaTime, OutwardsHookShotSpeed); 
	
	ServerRPC_ShootHook(HookArmLocation);

	// Hook has reached its target, start moving towards it 
	if(HookArmLocation.Equals(CurrentHookTargetLocation))
	{
		bShootingHookOutwards = false;

		// Start travelling towards the target if there is a valid target (did not hit a wall)
		if(CurrentTargetActor)
			StartTravelToTarget(); 
	}
}

void URobotHookingState::ServerRPC_ShootHook_Implementation(const FVector& NewHookEndLoc)
{
	if(!PlayerOwner->HasAuthority())
		return;

	HookArmLocation = NewHookEndLoc; 
}

void URobotHookingState::StartTravelToTarget()
{
	bTravellingTowardsTarget = true; 
	
	// Bind function to notify overlaps so damage can be dealt to enemies that are travelled over 
	PlayerOwner->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &URobotHookingState::ActorOverlap);

	ServerRPCStartTravel(); 
}

void URobotHookingState::ServerRPCStartTravel_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	// Invincible during hook shot, needs to be set on server 
	PlayerOwner->SetCanBeDamaged(false); 
	
	MulticastRPCStartTravel(); 
}

void URobotHookingState::MulticastRPCStartTravel_Implementation()
{
	Cast<ARobotStateMachine>(PlayerOwner)->OnHookShotTravelStart(); 
}

void URobotHookingState::TravelTowardsTarget(const float DeltaTime)
{
	// UE_LOG(LogTemp, Warning, TEXT("Travelling towards player"))

	if(FVector::Dist(PlayerOwner->GetActorLocation(), CurrentHookTargetLocation) < ReachedTargetDistTolerance)
	{
		// Check if Soul and trigger explosion event 
		if(bHookTargetIsSoul)
			CollidedWithSoul();
		
		EndHookShot();
		return; 
	}

	const FVector Direction = (CurrentHookTargetLocation - PlayerOwner->GetActorLocation()).GetSafeNormal();

	ServerTravelTowardsTarget(DeltaTime, Direction); 
}

void URobotHookingState::ServerTravelTowardsTarget_Implementation(const float DeltaTime, const FVector Direction)
{
	if(!PlayerOwner->HasAuthority())
		return;

	PlayerOwner->GetCharacterMovement()->Velocity = Direction * HookShotTravelSpeed;
}

void URobotHookingState::CollidedWithSoul()
{
	MovementComponent->Velocity = FVector::ZeroVector; 
	
	// UE_LOG(LogTemp, Warning, TEXT("Big explosion"))

	ServerRPCHookCollision(); 
}

void URobotHookingState::RetractHook(const float DeltaTime) 
{
	// Should only retract the hook if the player hit an obstacle, otherwise it "retracts" with the player moving to target 
	if(bTravellingTowardsTarget)
		return; 
	
	// Lerp hook arm location back towards the Robot TODO: Replace player loc with shoulder loc? 
	const FVector NewEndLoc = UKismetMathLibrary::VInterpTo_Constant(HookArmLocation, PlayerOwner->GetActorLocation(), DeltaTime, RetractHookOnMissSpeed); 

	ServerRPC_RetractHook(NewEndLoc);

	// Fully retracted hook, change back to base state 
	if(HookArmLocation.Equals(PlayerOwner->GetActorLocation())) 
		EndHookShot(); 
}

void URobotHookingState::ActorOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Do not damage players 
	if(!PlayerOwner->IsLocallyControlled() || Cast<APROJCharacter>(OtherActor))
		return;

	ServerRPC_DamageActor(OtherActor); 
}

void URobotHookingState::ServerRPC_DamageActor_Implementation(AActor* ActorToDamage)
{
	ActorToDamage->TakeDamage(HookTravelDamageAmount, FDamageEvent(), PlayerOwner->GetInstigatorController(), PlayerOwner); 
}

void URobotHookingState::ServerRPC_RetractHook_Implementation(const FVector& NewEndLocation)
{
	if(!PlayerOwner->HasAuthority())
		return;

	HookArmLocation = NewEndLocation; 
}

void URobotHookingState::MulticastRPCHookShotEnd_Implementation(ARobotStateMachine* RobotChar)
{
	PlayerOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); 

	RobotChar->OnHookShotEnd(); 
}

void URobotHookingState::ServerRPCHookShotEnd_Implementation(ARobotStateMachine* RobotChar, const FVector& NewVel)
{
	if(!PlayerOwner->HasAuthority())
		return;

	const auto MovementComp = PlayerOwner->GetCharacterMovement(); 

	MovementComp->GravityScale = DefaultGravityScale;
	
	MovementComp->Velocity = NewVel;

	bHookShotActive = false; 

	MulticastRPCHookShotEnd(RobotChar); 
}

void URobotHookingState::ServerRPCHookCollision_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	MulticastRPCHookCollision(); 
}

void URobotHookingState::MulticastRPCHookCollision_Implementation()
{
	/**
	 * Note: according to what I can gather, spawning on server should spawn on clients but does not seem to be the
	 * case here? So I'm spawning the explosion actor for all players instead.
	 * TODO: Object pooling? Different actor overkill? Just do damage etc. here directly instead? 
	 */
	
	// Source to spawn "with construct parameters": https://forums.unrealengine.com/t/spawning-an-actor-with-parameters/329151/6

	// Spawns the explosion actor and passes the relevant information 
	const FTransform SpawnTransform(FRotator::ZeroRotator, PlayerOwner->GetActorLocation());
	if (const auto ExplosionActor = Cast<AHookExplosionActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ExplosionClassToSpawnOnCollWithSoul, SpawnTransform)))
	{
		const float TravelDistance = FVector::Dist(StartLocation, PlayerOwner->GetActorLocation()); 
		ExplosionActor->Initialize(TravelDistance, PlayerOwner); 

		UGameplayStatics::FinishSpawningActor(ExplosionActor, SpawnTransform);
	}
	
	Cast<ARobotStateMachine>(PlayerOwner)->OnHookExplosion(); 
}
