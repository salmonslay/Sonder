// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStateMachine.h"

#include "DummyPlayerState.h"
#include "PlayerCharState.h"

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

void ACharacterStateMachine::UpdateStateInputComp() const
{
	if(CurrentState)
		CurrentState->UpdateInputCompOnEnter(GetInputComponent());
	else
		UE_LOG(LogTemp, Error, TEXT("No current state, %s - lcl ctrl: %i"), *GetActorNameOrLabel(), IsLocallyControlled())
}

void ACharacterStateMachine::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(CurrentState)
		CurrentState->Update(DeltaSeconds);
}

UPlayerCharState* ACharacterStateMachine::GetStartingState() const
{
	return DummyState; // This is placeholder and should not return the dummy state 
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
