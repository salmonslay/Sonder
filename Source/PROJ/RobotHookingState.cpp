// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotHookingState.h"

#include "CharacterStateMachine.h"
#include "RobotBaseState.h"
#include "RobotStateMachine.h"
#include "SoulCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CableComponent.h"
#include "HookExplosionActor.h"
#include "HookShotAttachment.h"
#include "StaticsHelper.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

void URobotHookingState::Enter()
{
	Super::Enter();

	if(!SoulCharacter)
		SoulCharacter = UGameplayStatics::GetActorOfClass(this, ASoulCharacter::StaticClass()); 
	
	if(!RobotCharacter)
		RobotCharacter = Cast<ARobotStateMachine>(PlayerOwner);

	if(!HookCable)
		HookCable = RobotCharacter->FindComponentByClass<UCableComponent>();

	if(!MovementComponent)
		MovementComponent = RobotCharacter->GetCharacterMovement();

	DefaultGravityScale = MovementComponent->GravityScale;

	FailSafeTimer = 0;

	StartLocation = PlayerOwner->GetActorLocation();

	// No blocking objects 
	if(SetHookTarget())
	{
		StartTravelToTarget(); 
		// UE_LOG(LogTemp, Warning, TEXT("Shooting towards target"))
	} else // Something blocked 
	{
		// Do we want to do something here? Event? 
		
		// UE_LOG(LogTemp, Warning, TEXT("Block between player"))
	}

	PlayerOwner->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &URobotHookingState::ActorOverlap); 
	
	ShootHook(); 
}

void URobotHookingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	if(bTravellingTowardsTarget)
	{
		FHitResult HitResult; 
		DoLineTrace(HitResult);
	
		// Perform line trace while travelling towards target to see if something is now blocking (for whatever reason)
		if(HitResult.IsValidBlockingHit())
		{
			UE_LOG(LogTemp, Warning, TEXT("Ended in update"))
			EndHookShot();
			return; 
		}
		
		TravelTowardsTarget(DeltaTime);
	}
	
	RetractHook(DeltaTime);

	// Fail safe to ensure player does not get stuck while using hook shot 
	if((FailSafeTimer += DeltaTime) >= FailSafeLength)
		EndHookShot();
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
}

void URobotHookingState::EndHookShot() const
{
	// Change state if this state is active 
	if(Cast<ARobotStateMachine>(GetOwner())->GetCurrentState() == this)
		PlayerOwner->SwitchState(RobotCharacter->RobotBaseState);
}

bool URobotHookingState::SetHookTarget()
{
	FHitResult HitResult;
	const AActor* HitActorTarget = DoLineTrace(HitResult);

	if(!SoulCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No soul in SetHookTarget"))
		CurrentHookTargetLocation = PlayerOwner->GetActorLocation(); 
		return false;
	}
	
	CurrentHookTargetLocation = HitActorTarget ? HitActorTarget->GetActorLocation() : HitResult.Location;

	// Offset target location if targeting Soul 
	if(HitActorTarget == SoulCharacter)
	{
		CurrentHookTargetLocation += FVector::UpVector * ZSoulTargetOffset;
		bHookTargetIsSoul = true; 
	} else
		bHookTargetIsSoul = false; 

	if(!StaticsHelper::ActorIsInFront(RobotCharacter, CurrentHookTargetLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Target not in front of player"))
		CurrentHookTargetLocation = GetTargetOnNothingInFront(); 
		return false; 
	}

	return !HitResult.IsValidBlockingHit(); 
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

AActor* URobotHookingState::DoLineTrace(FHitResult& HitResultOut)
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
	
	// GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Pawn, Params);

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
		
		return HookTarget; 
	}

	// No valid target 
	return nullptr; 
}

void URobotHookingState::ShootHook() 
{
	// Shoots the hook outwards

	ServerRPCHookShotStart(HookCable, CurrentHookTargetLocation, RobotCharacter); 
}

void URobotHookingState::StartTravelToTarget()
{
	bTravellingTowardsTarget = true;

	RobotCharacter->DisableInput(RobotCharacter->GetLocalViewingPlayerController());

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
	PlayerOwner->GetCharacterMovement()->GravityScale = 0;
	PlayerOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
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
	// Shooting towards Soul 
	if(bTravellingTowardsTarget)
	{
		const FVector EndLocRelToOwner = PlayerOwner->GetTransform().InverseTransformPosition(CurrentHookTargetLocation);
		ServerRPC_RetractHook(EndLocRelToOwner); 
	} else // Hit a blocking object 
	{
		const FVector DirToPlayer = (PlayerOwner->GetActorLocation() - CurrentHookTargetLocation).GetSafeNormal();
		CurrentHookTargetLocation += DirToPlayer * DeltaTime * RetractHookOnMissSpeed; // Move current target closer to player 

		// Set new end loc for the cable 
		const FVector EndLocRelToOwner = PlayerOwner->GetTransform().InverseTransformPosition(CurrentHookTargetLocation);

		ServerRPC_RetractHook(EndLocRelToOwner); 
	}

	// Fully retracted hook, change back to base state 
	if(HookCable->EndLocation.Equals(HookCable->GetRelativeLocation(), 10.f)) 
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

void URobotHookingState::MulticastRPC_RetractHook_Implementation(const FVector& NewEndLocation)
{
	GetOwner()->FindComponentByClass<UCableComponent>()->EndLocation = NewEndLocation; 
}

void URobotHookingState::ServerRPC_RetractHook_Implementation(const FVector& NewEndLocation)
{
	if(!PlayerOwner->HasAuthority())
		return;

	MulticastRPC_RetractHook(NewEndLocation); 
}

void URobotHookingState::MulticastRPCHookShotEnd_Implementation(ARobotStateMachine* RobotChar)
{
	GetOwner()->FindComponentByClass<UCableComponent>()->SetVisibility(false);
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

	MulticastRPCHookShotEnd(RobotChar); 
}

void URobotHookingState::MulticastRPCHookShotStart_Implementation(const FVector& HookTarget, ARobotStateMachine* RobotChar)
{
	if(!GetOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null"))
		return; 
	}
	
	const auto CableComp = GetOwner()->FindComponentByClass<UCableComponent>(); 

	if(!CableComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Cable comp is null"))
		return; 
	}
	
	CableComp->SetVisibility(true);
	CableComp->bAttachEnd = true;

	// End location is relative to owner
	const FVector EndLocRelToOwner = GetOwner()->GetTransform().InverseTransformPosition(HookTarget);
	
	// TODO: lerp this? Needs to be called in Update if so 
	CableComp->EndLocation = EndLocRelToOwner;

	RobotChar->OnHookShotStart(); 
}

void URobotHookingState::ServerRPCHookShotStart_Implementation(UCableComponent* HookCableComp, const FVector& HookTarget, ARobotStateMachine* RobotChar)
{
	if(!PlayerOwner->HasAuthority())
		return;

	MulticastRPCHookShotStart(HookTarget, RobotChar); 
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
