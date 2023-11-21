// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemyCharacter.h"

#include "Components/SpotLightComponent.h"


void AFlyingEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	
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

void AFlyingEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bSetFocusToPlayer)
	{
		if (CurrentTargetPlayer)
		{
			SetActorRotation((CurrentTargetPlayer->GetActorLocation() - GetActorLocation()).Rotation());
		}
	}
}
