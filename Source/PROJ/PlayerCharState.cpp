// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharState.h"

#include "PROJCharacter.h"

// Sets default values for this component's properties
UPlayerCharState::UPlayerCharState()
{
	// Tick is turned off, its "tick" is instead handled in the Update function 
	PrimaryComponentTick.bCanEverTick = false;

}

void UPlayerCharState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	PlayerInputComponent = InputComp; 
}

void UPlayerCharState::BeginPlay()
{
	Super::BeginPlay();

	PlayerOwner = Cast<APROJCharacter>(GetOwner());

	if(!PlayerOwner)
		UE_LOG(LogTemp, Error, TEXT("No player owner"))
}
