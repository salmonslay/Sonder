// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowSoulCharacter.h"

#include "SoulBaseStateNew.h"
#include "SoulDashingState.h"

AShadowSoulCharacter::AShadowSoulCharacter()
{
	SoulBaseState = CreateDefaultSubobject<USoulBaseStateNew>(TEXT("BaseState")); 
	DashState = CreateDefaultSubobject<USoulDashingState>(TEXT("DashState")); 
}
