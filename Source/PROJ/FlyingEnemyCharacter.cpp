// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemyCharacter.h"

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
