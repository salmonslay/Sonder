// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulBaseState.h"

#include "CharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "SoulCharacter.h"
#include "SoulDashingState.h"
#include "Net/UnrealNetwork.h"

void USoulBaseState::Enter()
{
	Super::Enter();

	if(!SoulCharacter)
		SoulCharacter = Cast<ASoulCharacter>(PlayerOwner);

	UE_LOG(LogTemp, Warning, TEXT("Entered soul base state, lcl ctrl: %i"), PlayerOwner->IsLocallyControlled()) 
}

void USoulBaseState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	
}

void USoulBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	if(!bHasSetUpInput)
	{
		InputComp->BindAction(DashInputAction, ETriggerEvent::Started, this, &USoulBaseState::Dash);
		bHasSetUpInput = true; 
	}
}

void USoulBaseState::Exit()
{
	Super::Exit();

	bDashCoolDownActive = true;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USoulBaseState::DisableDashCooldown, 1); 

	// Removing the action binding would require changing the action mapping 
	// PlayerInputComponent->RemoveActionBinding(DashInputAction, ETriggerEvent::Started); 
}

void USoulBaseState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USoulBaseState, DashCooldown)
}

void USoulBaseState::Dash()
{
	// Only run locally 
	if(bDashCoolDownActive || !PlayerOwner->IsLocallyControlled())
		return;

	PlayerOwner->SwitchState(SoulCharacter->DashingState); 
}
