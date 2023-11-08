// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacter.h"

#include "SoulBaseState.h"

ASoulCharacter::ASoulCharacter()
{
	BaseState = CreateDefaultSubobject<USoulBaseState>(FName("Soul Base State"));

	// CurrentState = BaseState; 
}

UPlayerCharState* ASoulCharacter::GetStartingState() const
{
	UE_LOG(LogTemp, Warning, TEXT("Sub starting state"))

	return BaseState; 
}
