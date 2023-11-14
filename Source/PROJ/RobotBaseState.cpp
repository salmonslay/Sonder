// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotBaseState.h"

#include "EnhancedInputComponent.h"
#include "RobotStateMachine.h"

void URobotBaseState::Enter()
{
	Super::Enter();

	if(!RobotCharacter)
		RobotCharacter = Cast<ARobotStateMachine>(PlayerOwner); 

	UE_LOG(LogTemp, Warning, TEXT("Entered robot base state, lcl ctrl: %i"), RobotCharacter->IsLocallyControlled())
}

void URobotBaseState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	// UE_LOG(LogTemp, Warning, TEXT("Updating robot base, lcl ctrl: %i, name: %s"), PlayerOwner->HasAuthority(), *PlayerOwner->GetActorNameOrLabel())
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

void URobotBaseState::ShootHook()
{
	UE_LOG(LogTemp, Warning, TEXT("Fired hook"))
}
