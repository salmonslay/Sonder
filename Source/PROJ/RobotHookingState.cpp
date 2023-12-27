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
#include "NiagaraFunctionLibrary.h"
#include "StaticsHelper.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"

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
		RobotCharacter = Cast<ARobotStateMachine>(CharOwner);

	if(!MovementComponent)
		MovementComponent = RobotCharacter->GetCharacterMovement();

	DefaultGravityScale = MovementComponent->GravityScale;

	FailSafeTimer = 0;

	StartLocation = CharOwner->GetActorLocation();

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
		CurrentHookTargetLocation = CurrentTargetActor->GetActorLocation(); 
		
		FHitResult HitResult; 
		AHookShotAttachment::GetCurrentTarget(HitResult); 
	
		// Perform line trace while travelling towards target to see if something is now blocking (for whatever reason)
		if(HitResult.IsValidBlockingHit())
		{
			EndHookShot();
			return; 
		}
		
		TravelTowardsTarget(DeltaTime);
	}

	RetractHook(DeltaTime); 
}

void URobotHookingState::Exit()
{
	Super::Exit();

	// UE_LOG(LogTemp, Warning, TEXT("Exited hook state"))

	if(!RobotCharacter->InputEnabled())
		RobotCharacter->EnableInput(RobotCharacter->GetLocalViewingPlayerController());

	CharOwner->GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &URobotHookingState::ActorOverlap);

	// Calculate new velocity, defaulting at current velocity 
	const auto MovementComp = CharOwner->GetCharacterMovement();
	FVector NewVel = MovementComp->Velocity; 
	
	// If targeted a static hook or Soul 
	if(bTravellingTowardsTarget) // Set velocity to zero if Soul otherwise keep some momentum 
		NewVel = bHookTargetIsSoul ? FVector::ZeroVector : MovementComp->Velocity / VelocityDivOnReachedHook;

	ServerRPCHookShotEnd(RobotCharacter, NewVel);
	
	bTravellingTowardsTarget = false;
	bHookTargetIsSoul = false; 
	bHookShotActive = false; 
}

void URobotHookingState::EndHookShot(const bool bEndFromDash) const
{
	// Dont end if called from dash and target is NOT Soul 
	if(bEndFromDash && !bHookTargetIsSoul)
		return; 
	
	// Change state if this state is active 
	if(Cast<ARobotStateMachine>(GetOwner())->GetCurrentState() == this)
		Cast<ACharacterStateMachine>(CharOwner)->SwitchState(RobotCharacter->RobotBaseState);
}

void URobotHookingState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URobotHookingState, CurrentHookTargetLocation)
	DOREPLIFETIME(URobotHookingState, bHookShotActive)
	DOREPLIFETIME(URobotHookingState, HookArmLocation)
	DOREPLIFETIME(URobotHookingState, bTravellingTowardsTarget)
}

bool URobotHookingState::SetHookTarget()
{
	FHitResult HitResult;
	CurrentTargetActor = AHookShotAttachment::GetCurrentTarget(HitResult); 

	if(!SoulCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No soul in SetHookTarget"))
		CurrentHookTargetLocation = CharOwner->GetActorLocation(); 
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

	if(!CurrentTargetActor || !UStaticsHelper::ActorIsInFront(RobotCharacter, CurrentHookTargetLocation))
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
	Params.AddIgnoredActor(CharOwner);

	FVector EndLoc = CharOwner->GetActorLocation() + CharOwner->GetActorForwardVector() * MaxHookShotDistanceOnBlock; 
	
	GetWorld()->LineTraceSingleByChannel(HitResult, CharOwner->GetActorLocation(), EndLoc, ECC_Pawn, Params);

	// Return hit location or end loc if no hit 
	return HitResult.IsValidBlockingHit() ? HitResult.Location : EndLoc; 
}

// Run once when hook shoots out 
void URobotHookingState::StartShootHook()
{
	// Disable input if there is a valid target 
	if(CurrentTargetActor)
		RobotCharacter->DisableInput(RobotCharacter->GetLocalViewingPlayerController());
	
	bShootingHookOutwards = true;
	bHookShotActive = true; 
	
	HookArmLocation = CharOwner->GetActorLocation(); 
	
	ServerRPCHookShotStart(RobotCharacter, IsValid(CurrentTargetActor)); 
}

void URobotHookingState::ServerRPCHookShotStart_Implementation(ARobotStateMachine* RobotChar, const bool bValidHookTarget)
{
	if(!CharOwner->HasAuthority())
		return;

	HookArmLocation = CharOwner->GetActorLocation(); 

	bHookShotActive = true; 

	MulticastRPCHookShotStart(RobotChar, bValidHookTarget); 
}

void URobotHookingState::MulticastRPCHookShotStart_Implementation(ARobotStateMachine* RobotChar, const bool bValidHookTarget)
{
	if(!GetOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null"))
		return; 
	}
	
	if(bValidHookTarget) 
	{
		if(CharOwner->GetCharacterMovement()->Velocity.Z < 0.f) 
			CharOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

		CharOwner->GetCharacterMovement()->GravityScale = 0;
		CharOwner->GetCharacterMovement()->Velocity = FVector::ZeroVector;
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
	if(!CharOwner->HasAuthority())
		return;

	HookArmLocation = NewHookEndLoc; 
}

void URobotHookingState::StartTravelToTarget()
{
	bTravellingTowardsTarget = true; 
	
	// Bind function to notify overlaps so damage can be dealt to enemies that are travelled over 
	CharOwner->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &URobotHookingState::ActorOverlap);

	ServerRPCStartTravel(); 
}

void URobotHookingState::ServerRPCStartTravel_Implementation()
{
	if(!CharOwner->HasAuthority())
		return;

	// Invincible during hook shot, needs to be set on server 
	CharOwner->SetCanBeDamaged(false);

	bTravellingTowardsTarget = true; 
	
	MulticastRPCStartTravel(); 
}

void URobotHookingState::MulticastRPCStartTravel_Implementation()
{
	if(GrabEffect)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, GrabEffect, CurrentHookTargetLocation);

	if(HookShotTravelEffect)
		HookShotTravelComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(HookShotTravelEffect, GetOwner()->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);

	CharOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying); 
	
	Cast<ARobotStateMachine>(CharOwner)->OnHookShotTravelStart(); 
}

void URobotHookingState::TravelTowardsTarget(const float DeltaTime)
{
	// UE_LOG(LogTemp, Warning, TEXT("Travelling towards player"))

	if(FVector::Dist(CharOwner->GetActorLocation(), CurrentHookTargetLocation) < ReachedTargetDistTolerance)
	{
		// Check if Soul and trigger explosion event 
		if(bHookTargetIsSoul)
			CollidedWithSoul();
		
		EndHookShot();
		return; 
	}

	const FVector Direction = (CurrentHookTargetLocation - CharOwner->GetActorLocation()).GetSafeNormal();

	ServerTravelTowardsTarget(DeltaTime, Direction); 
}

void URobotHookingState::ServerTravelTowardsTarget_Implementation(const float DeltaTime, const FVector Direction)
{
	if(!CharOwner->HasAuthority())
		return;

	CharOwner->GetCharacterMovement()->Velocity = Direction * HookShotTravelSpeed;
}

void URobotHookingState::CollidedWithSoul()
{
	MovementComponent->Velocity = FVector::ZeroVector; 
	
	// UE_LOG(LogTemp, Warning, TEXT("Big explosion"))

	ServerRPCHookCollision(); 
}

void URobotHookingState::RetractHook(const float DeltaTime) 
{
	if(bTravellingTowardsTarget)
	{
		// if travelling towards the target, simply set the hook arm to be placed at target location 
		ServerRPC_RetractHook(CurrentHookTargetLocation); 
		return;
	}
	
	// Lerp hook arm location back towards the Robot TODO: Replace player loc with shoulder/hand loc? 
	const FVector NewEndLoc = UKismetMathLibrary::VInterpTo_Constant(HookArmLocation, CharOwner->GetActorLocation(), DeltaTime, RetractHookOnMissSpeed); 

	// Fully retracted hook, change back to base state 
	if(FVector::DistSquared(NewEndLoc, CharOwner->GetActorLocation()) < 100)
	{
		EndHookShot();
		return;
	}
	
	ServerRPC_RetractHook(NewEndLoc);
}

void URobotHookingState::ActorOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Do not damage players 
	if(!CharOwner->IsLocallyControlled() || Cast<APROJCharacter>(OtherActor))
		return;

	ServerRPC_DamageActor(OtherActor); 
}

void URobotHookingState::ServerRPC_DamageActor_Implementation(AActor* ActorToDamage)
{
	ActorToDamage->TakeDamage(HookTravelDamageAmount, FDamageEvent(), CharOwner->GetInstigatorController(), CharOwner); 
}

void URobotHookingState::ServerRPC_RetractHook_Implementation(const FVector& NewEndLocation)
{
	if(!CharOwner->HasAuthority())
		return;

	HookArmLocation = NewEndLocation; 
}

void URobotHookingState::MulticastRPCHookShotEnd_Implementation(ARobotStateMachine* RobotChar)
{
	CharOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if(HookShotTravelComponent)
		HookShotTravelComponent->DestroyComponent(); 

	RobotChar->OnHookShotEnd(); 
}

void URobotHookingState::ServerRPCHookShotEnd_Implementation(ARobotStateMachine* RobotChar, const FVector& NewVel)
{
	if(!CharOwner->HasAuthority())
		return;

	const auto MovementComp = CharOwner->GetCharacterMovement(); 

	MovementComp->GravityScale = DefaultGravityScale;
	
	MovementComp->Velocity = NewVel;

	bHookShotActive = false;
	bTravellingTowardsTarget = false;

	CharOwner->SetCanBeDamaged(true);

	MulticastRPCHookShotEnd(RobotChar); 
}

void URobotHookingState::ServerRPCHookCollision_Implementation()
{
	if(!CharOwner->HasAuthority())
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
	const FTransform SpawnTransform(FRotator::ZeroRotator, CharOwner->GetActorLocation());
	if (const auto ExplosionActor = Cast<AHookExplosionActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ExplosionClassToSpawnOnCollWithSoul, SpawnTransform)))
	{
		const float TravelDistance = FVector::Dist(StartLocation, CharOwner->GetActorLocation()); 
		ExplosionActor->Initialize(TravelDistance, CharOwner); 

		UGameplayStatics::FinishSpawningActor(ExplosionActor, SpawnTransform);
	}
	
	Cast<ARobotStateMachine>(CharOwner)->OnHookExplosion(); 
}
