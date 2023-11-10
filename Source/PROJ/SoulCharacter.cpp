// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacter.h"

#include "SoulBaseState.h"
#include "SoulDashingState.h"

ASoulCharacter::ASoulCharacter()
{
	BaseState = CreateDefaultSubobject<USoulBaseState>(FName("Soul Base State"));

	DashingState = CreateDefaultSubobject<USoulDashingState>(FName("Dashing State")); 

	// CurrentState = BaseState; 
}

UPlayerCharState* ASoulCharacter::GetStartingState() const
{
	return BaseState; 
}
