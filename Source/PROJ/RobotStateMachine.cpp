// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotStateMachine.h"

#include "RobotBaseState.h"
#include "RobotHookingState.h"

ARobotStateMachine::ARobotStateMachine()
{
	RobotBaseState = CreateDefaultSubobject<URobotBaseState>(FName("BaseState"));
	
	HookState = CreateDefaultSubobject<URobotHookingState>(FName("HookState"));
}
