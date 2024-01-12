// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatTrigger.h"
#include "Components/BoxComponent.h"

ACombatTrigger::ACombatTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Bounds->SetCollisionProfileName("CombatTrigger");
	RootComponent = Bounds;
}
