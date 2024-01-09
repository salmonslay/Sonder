// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStateMachine.h"

#include "PlayerCharState.h"
#include "RobotBaseState.h"
#include "SoulBaseStateNew.h"
#include "Net/UnrealNetwork.h"

void ACharacterStateMachine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterStateMachine, CurrentState) 
}

void ACharacterStateMachine::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = GetStartingState();

	if(CurrentState)
		CurrentState->Enter();
}

void ACharacterStateMachine::UpdateStateInputComp() 
{
	if(!IsLocallyControlled())
		return; 

	if(CurrentState)
		CurrentState->UpdateInputCompOnEnter(GetInputComponent());
	else
	{
		CurrentState = GetStartingState();
		CurrentState->UpdateInputCompOnEnter(GetInputComponent()); 
	}

	if(!CurrentState)
		UE_LOG(LogTemp, Error, TEXT("No current state, %s - lcl ctrl: %i"), *GetActorNameOrLabel(), IsLocallyControlled())
}

void ACharacterStateMachine::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!IsLocallyControlled())
		return; 

	if(CurrentState)
		CurrentState->Update(DeltaSeconds);
}

UPlayerCharState* ACharacterStateMachine::GetStartingState() const
{
	if(const auto SoulBaseState = FindComponentByClass<USoulBaseStateNew>())
		return SoulBaseState;
	
	if(const auto RobotBaseState = FindComponentByClass<URobotBaseState>())
		return RobotBaseState;

	UE_LOG(LogTemp, Warning, TEXT("Error. %s has no base state"), *GetActorNameOrLabel())
	return nullptr; // Should not get here 
}

void ACharacterStateMachine::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UpdateStateInputComp(); 
}

void ACharacterStateMachine::ServerRPC_SwitchState_Implementation(UPlayerCharState* NewState)
{
	if(!HasAuthority())
		return;

	CurrentState = NewState; 
}

void ACharacterStateMachine::SwitchState(UPlayerCharState* NewState)
{
	if(CurrentState == NewState)
		return; 
	
	if(CurrentState)
		CurrentState->Exit();

	ServerRPC_SwitchState(NewState); 
	CurrentState = NewState;

	CurrentState->Enter();
	CurrentState->UpdateInputCompOnEnter(GetInputComponent()); 
}

