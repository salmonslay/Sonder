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

	// No blocking objects 
	if(SetHookTarget())
	{
		StartTravelToTarget(); 
		// UE_LOG(LogTemp, Warning, TEXT("Shooting towards player"))
	} else // Something blocked 
	{
		// Do we want to do something here? Event? 
		
		// UE_LOG(LogTemp, Warning, TEXT("Block between player"))
	}
	
	ShootHook(); 
}

void URobotHookingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	// Perform line trace while travelling towards target to see if something is now blocking (for whatever reason)
	if(bTravellingTowardsTarget && DoLineTrace().IsValidBlockingHit())
	{
		EndHookShot(); 
		return; 
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("Grav scale: %f"), MovementComponent->GravityScale)

	if(bTravellingTowardsTarget)
		TravelTowardsTarget(DeltaTime);
	
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
	
	ServerRPCHookShotEnd(HookCable, RobotCharacter, bTravellingTowardsTarget);
	
	bTravellingTowardsTarget = false;
}

void URobotHookingState::EndHookShot() const
{
	PlayerOwner->SwitchState(RobotCharacter->RobotBaseState); 
}

bool URobotHookingState::SetHookTarget()
{
	FHitResult HitResult = DoLineTrace();

	if(!SoulCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No soul in SetHookTarget"))
		CurrentHookTarget = PlayerOwner->GetActorLocation(); 
		return false;
	}
	
	CurrentHookTarget = HitResult.IsValidBlockingHit() ? HitResult.Location : SoulCharacter->GetActorLocation();

	CurrentHookTarget += FVector::UpVector * ZTargetOffset; 

	return !HitResult.IsValidBlockingHit(); 
}

FHitResult URobotHookingState::DoLineTrace()
{
	if(!SoulCharacter)
		SoulCharacter = UGameplayStatics::GetActorOfClass(this, ASoulCharacter::StaticClass()); 

	if(!SoulCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("No soul when attempting line trace"))
		return FHitResult(); 
	}
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActors( TArray<AActor*>( {PlayerOwner, SoulCharacter } ) ); // Ignore players

	const FVector StartLoc = PlayerOwner->GetActorLocation();
	const FVector EndLoc = SoulCharacter->GetActorLocation(); 
	
	// GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Pawn, Params);

	auto CapsuleComp = PlayerOwner->GetCapsuleComponent();
	
	// Sweep instead of Line Trace 
	FCollisionShape CollShape = FCollisionShape::MakeCapsule(CapsuleComp->GetScaledCapsuleRadius() / 2, CapsuleComp->GetScaledCapsuleHalfHeight() / 2);
	GetWorld()->SweepSingleByChannel(HitResult, StartLoc, EndLoc, FQuat::Identity, ECC_Pawn, CollShape, Params); 
	
	return HitResult; 
}

void URobotHookingState::ShootHook() 
{
	// Shoots the hook outwards

	ServerRPCHookShotStart(HookCable, CurrentHookTarget, RobotCharacter); 
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

	// MovementComponent->GravityScale = 0;
	
	MulticastRPCStartTravel(); 
}

void URobotHookingState::MulticastRPCStartTravel_Implementation()
{
	PlayerOwner->GetCharacterMovement()->GravityScale = 0;
}

void URobotHookingState::TravelTowardsTarget(const float DeltaTime)
{
	// UE_LOG(LogTemp, Warning, TEXT("Travelling towards player"))

	if(FVector::Dist(PlayerOwner->GetActorLocation(), CurrentHookTarget) < ReachedTargetDistTolerance)
	{
		CollidedWithSoul(); 
		EndHookShot(); 
		return; 
	}

	const FVector Direction = (CurrentHookTarget - PlayerOwner->GetActorLocation()).GetSafeNormal();

	ServerTravelTowardsTarget(DeltaTime, Direction); 
}

void URobotHookingState::ServerTravelTowardsTarget_Implementation(const float DeltaTime, const FVector Direction)
{
	if(!PlayerOwner->HasAuthority())
		return; 
	
	// PlayerOwner->SetActorLocation(PlayerOwner->GetActorLocation() + Direction * HookShotTravelSpeed * DeltaTime);

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
		const FVector EndLocRelToOwner = PlayerOwner->GetTransform().InverseTransformPosition(CurrentHookTarget);
		ServerRPC_RetractHook(EndLocRelToOwner); 
	} else // Hit a blocking object 
	{
		// TODO: Retract when it hits an obstacle, now simply changes state

		const FVector DirToPlayer = (PlayerOwner->GetActorLocation() - CurrentHookTarget).GetSafeNormal();
		CurrentHookTarget += DirToPlayer * DeltaTime * RetractHookOnMissSpeed; // Move current target closer to player 

		// Set new end loc for the cable 
		const FVector EndLocRelToOwner = PlayerOwner->GetTransform().InverseTransformPosition(CurrentHookTarget);

		ServerRPC_RetractHook(EndLocRelToOwner); 
		// HookCable->EndLocation = EndLocRelToOwner; 
		
		// EndHookShot(); 
	}

	// Fully retracted hook, change back to base state 
	if(HookCable->EndLocation.Equals(HookCable->GetRelativeLocation(), 10.f)) 
		EndHookShot(); 
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

	RobotChar->OnHookShotEnd(); 
}

void URobotHookingState::ServerRPCHookShotEnd_Implementation(UCableComponent* HookCableComp, ARobotStateMachine* RobotChar, const bool bResetVel)
{
	if(!PlayerOwner->HasAuthority())
		return;

	const auto MovementComp = PlayerOwner->GetCharacterMovement(); 

	MovementComp->GravityScale = DefaultGravityScale;

	if(bResetVel)
		MovementComp->Velocity = FVector::ZeroVector;

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
	Cast<ARobotStateMachine>(PlayerOwner)->OnHookExplosion(); 
}
