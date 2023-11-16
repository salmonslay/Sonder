// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulBaseStateNew.h"

#include "CharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "LightGrenade.h"
#include "PROJCharacter.h"
#include "SoulCharacter.h"
#include "SoulDashingState.h"

void USoulBaseStateNew::Enter()
{
	Super::Enter();

	if(!SoulCharacter)
		SoulCharacter = Cast<ASoulCharacter>(PlayerOwner);

	// UE_LOG(LogTemp, Warning, TEXT("Entered soul base state, lcl ctrl: %i"), PlayerOwner->IsLocallyControlled()) 
}

void USoulBaseStateNew::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	
}

void USoulBaseStateNew::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	if(!bHasSetUpInput)
	{
		InputComp->BindAction(DashInputAction, ETriggerEvent::Started, this, &USoulBaseStateNew::Dash);
		InputComp->BindAction(ThrowGrenadeInputAction,ETriggerEvent::Started,this,&USoulBaseStateNew::ThrowGrenade);
		bHasSetUpInput = true; 
	}
}

void USoulBaseStateNew::Exit()
{
	Super::Exit();

	bDashCoolDownActive = true;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USoulBaseStateNew::DisableDashCooldown, 1); 

	// Removing the action binding would require changing the action mapping 
	// PlayerInputComponent->RemoveActionBinding(DashInputAction, ETriggerEvent::Started); 
}

void USoulBaseStateNew::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void USoulBaseStateNew::Dash()
{
	// Only run locally 
	if(bDashCoolDownActive || !PlayerOwner->IsLocallyControlled())
		return;

	PlayerOwner->SwitchState(SoulCharacter->DashingState); 
}

void USoulBaseStateNew::ThrowGrenade()
{
	UE_LOG(LogTemp, Warning, TEXT("Throw Grenade"));
	if (!LightGrenade)
	{
		LightGrenade = GetWorld()->SpawnActor<ALightGrenade>();
	}
	LightGrenade->Throw();
}
