// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowRobotCharacter.h"

#include "RobotBaseState.h"
#include "RobotHookingState.h"


// Sets default values
AShadowRobotCharacter::AShadowRobotCharacter()
{
	RobotBaseState = CreateDefaultSubobject<URobotBaseState>(TEXT("Robot Base State"));
	RobotHookState = CreateDefaultSubobject<URobotHookingState>(TEXT("Robot Hook State")); 
}
