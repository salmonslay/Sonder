// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROJGameMode.h"
#include "PROJCharacter.h"
#include "UObject/ConstructorHelpers.h"

APROJGameMode::APROJGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
