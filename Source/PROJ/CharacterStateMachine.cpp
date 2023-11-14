// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStateMachine.h"

#include "DummyPlayerState.h"
#include "PlayerCharState.h"
#include "RobotBaseState.h"
#include "SoulBaseStateNew.h"

ACharacterStateMachine::ACharacterStateMachine()
{
	// Create the states 
	DummyState = CreateDefaultSubobject<UDummyPlayerState>(TEXT("DummyStateNew"));

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
	// UE_LOG(LogTemp, Warning, TEXT("Updating input comp new, %s"), *GetActorNameOrLabel())

	if(CurrentState)
		CurrentState->UpdateInputCompOnEnter(GetInputComponent());
	else
	{
		CurrentState = GetStartingState();
		CurrentState->UpdateInputCompOnEnter(GetInputComponent());
		// UE_LOG(LogTemp, Error, TEXT("No current state, %s - lcl ctrl: %i"), *GetActorNameOrLabel(), IsLocallyControlled())
	}
}

void ACharacterStateMachine::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
	return DummyState; // Should not get here 
}

void ACharacterStateMachine::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UpdateStateInputComp(); 
}

void ACharacterStateMachine::SwitchState(UPlayerCharState* NewState)
{
	if(CurrentState == NewState)
		return; 
	
	if(CurrentState)
		CurrentState->Exit();

	CurrentState = NewState;

	CurrentState->Enter();
	CurrentState->UpdateInputCompOnEnter(GetInputComponent()); 
} 
