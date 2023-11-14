// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotBaseState.h"

#include "EnhancedInputComponent.h"
#include "RobotHookingState.h"
#include "RobotStateMachine.h"

void URobotBaseState::Enter()
{
	Super::Enter();

	if(!RobotCharacter)
		RobotCharacter = Cast<ARobotStateMachine>(PlayerOwner);
}

void URobotBaseState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	
}

void URobotBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	// UE_LOG(LogTemp, Warning, TEXT("Setting up input, lcl ctrl: %i"), PlayerOwner->IsLocallyControlled())

	if(!bHasSetUpInput)
	{
		InputComp->BindAction(HookShotInputAction, ETriggerEvent::Started, this, &URobotBaseState::ShootHook);
		bHasSetUpInput = true; 
	}
}

void URobotBaseState::Exit()
{
	Super::Exit();

	
}

void URobotBaseState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void URobotBaseState::ShootHook()
{
	if(bHookShotOnCooldown)
		return; 
	
	// UE_LOG(LogTemp, Warning, TEXT("Fired hook"))

	bHookShotOnCooldown = true;
	
	// Enable hook shot after set time 
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &URobotBaseState::DisableHookShotCooldown, HookShotCooldownDelay); 
	
	PlayerOwner->SwitchState(RobotCharacter->HookState);
}
