// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacter.h"

#include "SoulBaseStateNew.h"
#include "SoulDashingState.h"

ASoulCharacter::ASoulCharacter()
{
	BaseStateNew = CreateDefaultSubobject<USoulBaseStateNew>(FName("New Soul Base State"));
	
	DashingState = CreateDefaultSubobject<USoulDashingState>(FName("DashingState"));

	ThrowLoc = CreateDefaultSubobject<USphereComponent>(FName("ThrowLoc"));
	ThrowLoc->SetupAttachment(RootComponent);
}
