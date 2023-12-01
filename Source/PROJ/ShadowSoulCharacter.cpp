// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowSoulCharacter.h"

#include "SoulBaseStateNew.h"
#include "SoulDashingState.h"

AShadowSoulCharacter::AShadowSoulCharacter()
{
	SoulBaseState = CreateDefaultSubobject<USoulBaseStateNew>(TEXT("SoulBaseState")); 
	DashState = CreateDefaultSubobject<USoulDashingState>(TEXT("DashState")); 
}

void AShadowSoulCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShadowSoulCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
