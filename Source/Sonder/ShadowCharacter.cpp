// Fill out your copyright notice in the Description page of Project Settings.

#include "ShadowCharacter.h"

#include "BasicAttackComponent.h"
#include "EnemyJumpTrigger.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharState.h"
#include "RobotBaseState.h"
#include "SoulBaseStateNew.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


#if !UE_BUILD_SHIPPING
namespace Jump
{
	bool bDebugJumpMovement = false;
	FAutoConsoleVariable CVarbDebugJumpMovement = {TEXT("sonder.bDebugJumpMovement"), bDebugJumpMovement, TEXT("Show and draw jump movement"), ECVF_Cheat};
	
}
#endif

AShadowCharacter::AShadowCharacter()
{
	EnemyAttackComp = CreateDefaultSubobject<UBasicAttackComponent>(TEXT("Basic Attack Comp"));
	EnemyAttackComp->SetupAttachment(RootComponent);
}

void AShadowCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShadowCharacter, CurrentState)
	DOREPLIFETIME(AShadowCharacter, bIsPerformingJump)
}

bool AShadowCharacter::CheckIfJumpNeeded()
{
	if (CurrentJumpTrigger == nullptr)
	{
		return false;
	}
	
	if (CurrentJumpTrigger->HasPathBetweenJumpPoints)
	{
		return false;
	}

	if (bHasLandedOnPlatform || bHasLandedOnGround)
	{
		return true;
	}
	return false;
}

void AShadowCharacter::MakeJump()
{
	if(GetLocalRole() == ROLE_Authority && !bIsStunned)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		//UE_LOG(LogTemp, Error, TEXT("MovementMode falling"));
		bCanBasicJump = false;
		bIsJumping = true;
		bIsPerformingJump = true;
		bHasLandedOnPlatform = false;
		bHasLandedOnGround = false;
		OnJumpEvent();
	}
}

void AShadowCharacter::Idle()
{
	Super::Idle();
	
	if(GetLocalRole() == ROLE_Authority && !bIsStunned)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		//UE_LOG(LogTemp, Error, TEXT("MovementMode walking"));
		bIsPerformingJump = false;
		bIsJumping = false;
	}
}

void AShadowCharacter::OnRep_Jump()
{
	if (bIsPerformingJump)
	{
		OnJumpEvent();
	}
	else
	{
		Idle();
	}
}

void AShadowCharacter::JumpToPoint(const FVector &JumpPoint)
{
	bIsJumping = true;
	FVector OutVel;
	GetMovementComponent()->Velocity = FVector(0.f, 0.f, 0.f);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVel, CurrentLocation, JumpPoint, 0, 0.6);
	GetCharacterMovement()->AddImpulse(OutVel.GetSafeNormal() * JumpBoost);
	MakeJump();
}


bool AShadowCharacter::IsNearlyAtLocation(const FVector& Loc) const
{
	return FMath::IsNearlyEqual(CurrentLocation.Y, Loc.Y, 20.f) && FMath::IsNearlyEqual(CurrentLocation.Z, Loc.Z, 20.f);
}


bool AShadowCharacter::HasNavigationToTarget(const FVector &CurrentTargetPoint) const
{
	const UNavigationSystemV1* Navigation = UNavigationSystemV1::GetCurrent(GetWorld());

	if (ensure(IsValid(Navigation))) {
		const UNavigationPath* NavigationPath = Navigation->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), CurrentTargetPoint);

		if(NavigationPath == nullptr)
		{
			return false;
		}
		const bool IsNavigationValid = NavigationPath->IsValid();
		const bool IsNavigationNotPartial = NavigationPath->IsPartial() == false;
		const bool IsNavigationSuccessful = IsNavigationValid && IsNavigationNotPartial;
		return IsNavigationSuccessful;
	}
	return false;
}

bool AShadowCharacter::PointCloserToPlayer(const FVector &CurrentTargetPoint) const
{
	return FVector::Distance(AvaliableJumpPoint, CurrentTargetPoint) < FVector::Distance(GetActorLocation(),CurrentTargetPoint);
}

bool AShadowCharacter::IsLeveledWithLocation(const FVector& Location) const
{
	return FMath::IsNearlyEqual(Location.Z, CurrentLocation.Z, MaxHeightDifferenceToMarkAsLeveled);
}

bool AShadowCharacter::IsWithinRangeFrom(const FVector& Location) const
{
	return FVector::Distance(Location, CurrentLocation) <= MaxDistanceToMarkAsReachable;
}


void AShadowCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = GetStartingState();

	if(CurrentState)
		CurrentState->Enter();

	JumpCoolDownTimer = JumpCoolDownDuration;
}

void AShadowCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!IsLocallyControlled())
	{
		return;
	}

	if(CurrentState)
	{
		CurrentState->Update(DeltaSeconds);
	}
	
	CurrentLocation = GetActorLocation();
	
	if (IsOverlappingWithTrigger && JumpCoolDownTimer >= JumpCoolDownDuration)
	{
		if (CheckIfJumpNeeded())
		{
			if (!bIsPerformingJump)
			{
				bCanBasicJump = true;
				AvaliableJumpPoint = CurrentJumpTrigger->RequestJumpLocation(GetActorLocation(), CurrentTargetLocation, ClosestJumpPoint, bHasLandedOnPlatform);
			}
		}
	}

#if !UE_BUILD_SHIPPING
	if (Jump::bDebugJumpMovement)
	{
		FString MovementModeString;
		if (GetCharacterMovement()->MovementMode == MOVE_Falling)
		{

			UE_LOG(LogTemp, Error, TEXT("Falling"));
			MovementModeString = TEXT("Falling");
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Walking"));
			MovementModeString = TEXT("Walking");
		}
		DrawDebugString(GetWorld(), CurrentLocation + FVector(0.f, 0.f, 100.f), MovementModeString, nullptr, FColor::White, 0.1f);
	}
#endif
	
}

UPlayerCharState* AShadowCharacter::GetStartingState() const
{
	if(const auto SoulBaseState = FindComponentByClass<USoulBaseStateNew>())
		return SoulBaseState;
	
	if(const auto RobotBaseState = FindComponentByClass<URobotBaseState>())
		return RobotBaseState;

	UE_LOG(LogTemp, Warning, TEXT("Error. %s has no base state"), *GetActorNameOrLabel())
	return nullptr; // Should not get here 
}

void AShadowCharacter::ServerRPC_SwitchState_Implementation(UPlayerCharState* NewState)
{
	if(!HasAuthority())
		return;

	CurrentState = NewState; 
}

void AShadowCharacter::SwitchState(UPlayerCharState* NewState)
{
	if(CurrentState == NewState)
		return; 
	
	if(CurrentState)
		CurrentState->Exit();

	ServerRPC_SwitchState(NewState); 
	CurrentState = NewState;

	CurrentState->Enter();
}

void AShadowCharacter::ServerRPC_ToggleChargeEffect_Implementation(const bool bActive)
{
	if(!HasAuthority())
		return;

	MulticastRPC_ToggleChargeEffect(bActive); 
}

void AShadowCharacter::MulticastRPC_ToggleChargeEffect_Implementation(const bool bActive)
{
	if(!ChargeEffect)
		return;

	// Create the effect if not already created 
	if(!ChargeEffectComp)
		ChargeEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ChargeEffect, GetRootComponent(), NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);

	bActive ? ChargeEffectComp->Activate(true) : ChargeEffectComp->Deactivate(); 
}
