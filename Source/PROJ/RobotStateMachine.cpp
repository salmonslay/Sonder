// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotStateMachine.h"

#include "RobotBaseState.h"
#include "RobotHookingState.h"

ARobotStateMachine::ARobotStateMachine()
{
	
	RobotBaseState = CreateDefaultSubobject<URobotBaseState>(FName("Robots Base State"));
	
	HookState = CreateDefaultSubobject<URobotHookingState>(FName("Hooks State")); 
}

void ARobotStateMachine::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Robot begin play, lcl ctrl: %i, name: %s"), IsLocallyControlled(), *GetActorNameOrLabel())
}

void ARobotStateMachine::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("Input robot, lcl ctrl: %i, name: %s"), IsLocallyControlled(), *GetActorNameOrLabel())
}
