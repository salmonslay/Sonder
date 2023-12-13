// Fill out your copyright notice in the Description page of Project Settings.

#include "ShadowCharacter.h"

#include "DummyPlayerState.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerBasicAttack.h"
#include "PlayerCharState.h"
#include "RobotBaseState.h"
#include "SoulBaseStateNew.h"
#include "Net/UnrealNetwork.h"

AShadowCharacter::AShadowCharacter()
{
	// Create the states 
	DummyState = CreateDefaultSubobject<UDummyPlayerState>(TEXT("DummyStateNew"));

	EnemyBasicAttack = CreateDefaultSubobject<UPlayerBasicAttack>(TEXT("BasicAttack"));
	EnemyBasicAttack->SetupAttachment(RootComponent);

	EnemyBasicAttack->SetRelativeLocation(FVector(50, 0, 0)); 
	EnemyBasicAttack->SetRelativeScale3D(FVector(1.5f, 1.5f, 2.f));
	EnemyBasicAttack->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 

}

void AShadowCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShadowCharacter, CurrentState)
	DOREPLIFETIME(AShadowCharacter, bIsPerformingJump)
}

void AShadowCharacter::MakeJump()
{
	if(GetLocalRole() == ROLE_Authority && !bIsStunned)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		UE_LOG(LogTemp, Error, TEXT("MovementMode falling"));
		
		bIsPerformingJump = true;
		OnJumpEvent();
	}
}

void AShadowCharacter::Idle()
{
	Super::Idle();
	
	if(GetLocalRole() == ROLE_Authority && !bIsStunned)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		bIsPerformingJump = false;
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


void AShadowCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = GetStartingState();

	if(CurrentState)
		CurrentState->Enter();

	
}

void AShadowCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!IsLocallyControlled())
		return; 

	if(CurrentState)
		CurrentState->Update(DeltaSeconds);

	
}

UPlayerCharState* AShadowCharacter::GetStartingState() const
{
	if(const auto SoulBaseState = FindComponentByClass<USoulBaseStateNew>())
		return SoulBaseState;
	
	if(const auto RobotBaseState = FindComponentByClass<URobotBaseState>())
		return RobotBaseState;

	UE_LOG(LogTemp, Warning, TEXT("Error. %s has no base state"), *GetActorNameOrLabel())
	return DummyState; // Should not get here 
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
