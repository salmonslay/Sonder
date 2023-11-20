// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacter.h"

#include "SoulBaseStateNew.h"
#include "SoulDashingState.h"

ASoulCharacter::ASoulCharacter()
{
	BaseStateNew = CreateDefaultSubobject<USoulBaseStateNew>(FName(TEXT("Soul Base State V2"))); 

	DashingState = CreateDefaultSubobject<USoulDashingState>(FName("Dashing State"));

	ThrowLoc = CreateDefaultSubobject<USphereComponent>(FName("ThrowLoc"));
	ThrowLoc->SetupAttachment(RootComponent);

}
