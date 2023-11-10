// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulBaseState.h"

#include "CharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "SoulCharacter.h"
#include "SoulDashingState.h"

void USoulBaseState::Enter()
{
	Super::Enter();

	if(!SoulCharacter)
		SoulCharacter = Cast<ASoulCharacter>(PlayerOwner); 
}

void USoulBaseState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);
	
}

void USoulBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);
	
	InputComp->BindAction(DashInputAction, ETriggerEvent::Started, this, &USoulBaseState::Dash);
}

void USoulBaseState::Exit()
{
	Super::Exit();

	// PlayerInputComponent->RemoveActionBinding(DashInputAction, ETriggerEvent::Started); 
}

void USoulBaseState::Dash()
{
	// Only run locally 
	if(!PlayerOwner->IsLocallyControlled())
		return;
	
	PlayerOwner->SwitchState(SoulCharacter->DashingState); 
}

void USoulBaseState::MulticastRPCDash_Implementation()
{
	// UE_LOG(LogTemp, Warning, TEXT("Multicast dash"))

	
}

void USoulBaseState::ServerRPCDash_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Told server that player dashed"))
}
