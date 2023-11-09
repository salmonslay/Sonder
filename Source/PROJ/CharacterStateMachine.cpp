// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStateMachine.h"

#include "DummyPlayerState.h"
#include "PlayerCharState.h"

ACharacterStateMachine::ACharacterStateMachine()
{
	// Create the states 
	DummyState = CreateDefaultSubobject<UDummyPlayerState>(TEXT("Dummy State"));

	// Set starting state 
	CurrentState = DummyState;
}

void ACharacterStateMachine::BeginPlay()
{
	Super::BeginPlay();

	CurrentState->Enter(); 
}

void ACharacterStateMachine::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentState->Update(DeltaSeconds); 
}

void ACharacterStateMachine::SwitchState(UPlayerCharState* NewState)
{
	CurrentState->Exit();

	CurrentState = NewState;

	NewState->Enter(); 
} 
