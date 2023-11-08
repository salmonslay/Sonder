// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulBaseState.h"

#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"

void USoulBaseState::Enter()
{
	Super::Enter();


}

void USoulBaseState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	
}

void USoulBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	if(InputComp)
		InputComp->BindAction(DashInputAction, ETriggerEvent::Started, this, &USoulBaseState::Dash);
	else
		UE_LOG(LogTemp, Error, TEXT("No input comp"))
}

void USoulBaseState::Exit()
{
	Super::Exit();

	// PlayerInputComponent->RemoveActionBinding(DashInputAction, ETriggerEvent::Started); 
}

void USoulBaseState::Dash()
{
	UE_LOG(LogTemp, Warning, TEXT("Dash"))
	
	// Only run locally 
	if(!PlayerOwner->IsLocallyControlled())
		return; 
	
}
