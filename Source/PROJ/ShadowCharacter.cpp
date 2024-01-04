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
#include "Net/UnrealNetwork.h"


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
		UE_LOG(LogTemp, Error, TEXT("Has set overlapping trigger but does not have pointer to that trigger"))
		return false;
	}
	
	if (CurrentJumpTrigger->HasPathBetweenJumpPoints)
	{
		UE_LOG(LogTemp, Error, TEXT("Has path between points and is a static jump trigger, no need for jump"))
		return false;
	}

	if (bHasLandedOnPlatform || bHasLandedOnGround)
	{
		//UE_LOG(LogTemp, Error, TEXT("Jump is needed, is grounded and is overlapping a jumptrigger with no path between jump points"))
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
		return; 

	if(CurrentState)
		CurrentState->Update(DeltaSeconds);
	
	if (IsOverlappingWithTrigger && JumpCoolDownTimer >= JumpCoolDownDuration)
	{
		if (CheckIfJumpNeeded())
		{
			if (!bIsPerformingJump)
			{
				bCanBasicJump = true;
				AvaliableJumpPoint = CurrentJumpTrigger->RequestJumpLocation(GetActorLocation(), CurrentTargetLocation, bHasLandedOnPlatform);
			}
		}
	}
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
