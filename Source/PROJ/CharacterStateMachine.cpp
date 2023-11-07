// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterStateMachine.h"

#include "DummyPlayerState.h"
#include "EnhancedInputComponent.h"
#include "PlayerBasicAttack.h"
#include "PlayerCharState.h"

ACharacterStateMachine::ACharacterStateMachine()
{
	// Create the states 
	DummyState = CreateDefaultSubobject<UDummyPlayerState>(TEXT("Dummy State"));

	// Set starting state 
	CurrentState = DummyState;

	BasicAttack = CreateDefaultSubobject<UPlayerBasicAttack>(FName("Basic Attack")); 
	BasicAttack->SetupAttachment(RootComponent); 
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

void ACharacterStateMachine::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		FindComponentByClass<UPlayerBasicAttack>()->SetUpInput(EnhancedInputComponent); 
	}
}

void ACharacterStateMachine::SwitchState(UPlayerCharState* NewState)
{
	CurrentState->Exit();

	CurrentState = NewState;

	NewState->Enter(); 
} 
