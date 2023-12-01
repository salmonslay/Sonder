// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowRobotCharacter.h"

#include "RobotBaseState.h"


// Sets default values
AShadowRobotCharacter::AShadowRobotCharacter()
{
	RobotBaseState = CreateDefaultSubobject<URobotBaseState>(TEXT("Robot Base State")); 
}

// Called when the game starts or when spawned
void AShadowRobotCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShadowRobotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
