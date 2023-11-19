// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemyCharacter.h"

#include "Components/SpotLightComponent.h"

void AFlyingEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	Laser = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));
}

void AFlyingEnemyCharacter::SetPointerToPath(const TArray<FVector>* PathPointer)
{
	if (PathPointer)
	{
		CurrentPath = *PathPointer;
	}
}

bool AFlyingEnemyCharacter::IsPathValid() const
{
	return !CurrentPath.IsEmpty();
}
