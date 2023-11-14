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

void URobotHookingState::Enter()
{
	Super::Enter();

	UE_LOG(LogTemp, Warning, TEXT("Entered hook state"))
	
	if(!SoulCharacter)
		SoulCharacter = UGameplayStatics::GetActorOfClass(this, ASoulCharacter::StaticClass()); 

	if(!RobotCharacter)
		RobotCharacter = Cast<ARobotStateMachine>(PlayerOwner);

	if(!HookCable)
		HookCable = PlayerOwner->FindComponentByClass<UCableComponent>(); 

	MovementComponent = RobotCharacter->GetCharacterMovement();

	DefaultGravityScale = MovementComponent->GravityScale; 

	// No blocking objects 
	if(SetHookTarget())
	{
		StartTravelToTarget(); 
		UE_LOG(LogTemp, Warning, TEXT("Shooting towards player"))
	} else // Something blocked 
	{
		UE_LOG(LogTemp, Warning, TEXT("Block between player"))
	}
	
	ShootHook(); 
}

void URobotHookingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	if(bTravellingTowardsTarget)
		TravelTowardsTarget(DeltaTime);
	
	RetractHook(DeltaTime);
}

void URobotHookingState::Exit()
{
	Super::Exit();

	bTravellingTowardsTarget = false;

	RobotCharacter->EnableInput(RobotCharacter->GetLocalViewingPlayerController());

	HookCable->SetVisibility(false); 

	// TODO: Might need to run on server or server and client 
	MovementComponent->GravityScale = 1.75f;

	ServerRPCHookShotEnd(); 
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
	
	CurrentHookTarget = HitResult.IsValidBlockingHit() ? HitResult.Location : SoulCharacter->GetActorLocation() + FVector::UpVector * PlayerOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2;; 

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
	
	// auto CapsuleComp = PlayerOwner->GetCapsuleComponent(); 

	// FVector StartLoc = PlayerOwner->GetActorLocation() + FVector::UpVector * CapsuleComp->GetScaledCapsuleHalfHeight() * 2;
	// FVector EndLoc = SoulCharacter->GetActorLocation() + FVector::UpVector * CapsuleComp->GetScaledCapsuleHalfHeight() * 2;
	
	// GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Pawn, Params);
	GetWorld()->LineTraceSingleByChannel(HitResult, PlayerOwner->GetActorLocation(), SoulCharacter->GetActorLocation(), ECC_Pawn, Params);

	// Sweep instead of Line Trace 
	// FCollisionShape CollShape = FCollisionShape::MakeCapsule(CapsuleComp->GetScaledCapsuleRadius() - 10, CapsuleComp->GetScaledCapsuleHalfHeight() - 10);
	// GetWorld()->SweepSingleByChannel(HitResult, StartLoc, EndLoc, FQuat::Identity, ECC_Pawn, CollShape, Params); 
	
	return HitResult; 
}

void URobotHookingState::ShootHook() 
{
	// Shoots the hook outwards
	
	HookCable->SetVisibility(true);
	HookCable->bAttachEnd = true;

	// End location is relative to owner
	const FVector EndLocRelToOwner = PlayerOwner->GetTransform().InverseTransformPosition(CurrentHookTarget);
	
	// TODO: lerp this? Needs to be called in Update if so 
	HookCable->EndLocation = EndLocRelToOwner;

	// If using cable length in any way, it does not update automatically relative to its end location 
	// HookCable->CableLength = FVector::Dist(HookCable->GetRelativeLocation(), EndLocRelToOwner);

	ServerRPCHookShotStart(); 
}

void URobotHookingState::StartTravelToTarget()
{
	bTravellingTowardsTarget = true;

	RobotCharacter->DisableInput(RobotCharacter->GetLocalViewingPlayerController());

	// TODO: This might need to run on both server and client, or at least ensure on server 
	MovementComponent->GravityScale = 0;
}

void URobotHookingState::TravelTowardsTarget(const float DeltaTime)
{
	// UE_LOG(LogTemp, Warning, TEXT("Travelling towards player"))

	if(FVector::Dist(PlayerOwner->GetActorLocation(), CurrentHookTarget) < ReachedTargetDistTolerance)
	{
		PlayerOwner->SwitchState(RobotCharacter->RobotBaseState);
		CollidedWithSoul(); 
		return; 
	}

	const FVector Direction = (CurrentHookTarget - PlayerOwner->GetActorLocation()).GetSafeNormal();

	// TODO: Make sure this runs on server? 
	PlayerOwner->SetActorLocation(PlayerOwner->GetActorLocation() + Direction * HookShotTravelSpeed * DeltaTime); 
}

void URobotHookingState::CollidedWithSoul()
{
	MovementComponent->Velocity = FVector::ZeroVector; 
	
	UE_LOG(LogTemp, Warning, TEXT("Big explosion"))

	ServerRPCHookCollision(); 
}

void URobotHookingState::ServerRPCHookCollision_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	MulticastRPCHookCollision(); 
}

void URobotHookingState::MulticastRPCHookCollision_Implementation()
{
	if(!PlayerOwner->IsLocallyControlled())
		return;

	RobotCharacter->OnHookExplosion(); 
}

void URobotHookingState::RetractHook(const float DeltaTime) const
{
	// Shooting towards Soul 
	if(bTravellingTowardsTarget)
	{
		const FVector EndLocRelToOwner = PlayerOwner->GetTransform().InverseTransformPosition(CurrentHookTarget);
		HookCable->EndLocation = EndLocRelToOwner; 
	} else 
	{
		// TODO: Retract when it hits an obstacle, now simply changes state 
		PlayerOwner->SwitchState(RobotCharacter->RobotBaseState);
	}

	// Fully retracted hook, change back to base state 
	if(HookCable->EndLocation.Equals(HookCable->GetRelativeLocation(), 10.f)) 
		PlayerOwner->SwitchState(RobotCharacter->RobotBaseState);

	// UE_LOG(LogTemp, Warning, TEXT("Retracting hook, dist: %f"), FVector::Dist(HookCable->GetRelativeLocation(), HookCable->EndLocation))
}

void URobotHookingState::MulticastRPCHookShotEnd_Implementation()
{
	if(!PlayerOwner->IsLocallyControlled())
		return;

	RobotCharacter->OnHookShotEnd(); 
}

void URobotHookingState::ServerRPCHookShotEnd_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	MulticastRPCHookShotEnd(); 
}

void URobotHookingState::MulticastRPCHookShotStart_Implementation()
{
	if(!PlayerOwner->IsLocallyControlled())
		return;

	RobotCharacter->OnHookShotStart(); 
}

void URobotHookingState::ServerRPCHookShotStart_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	MulticastRPCHookShotStart(); 
}
