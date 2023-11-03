// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROJGameMode.h"
#include "PROJCharacter.h"
#include "ProjPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

APROJGameMode::APROJGameMode()
{
	// This is from UE auto generation 
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default player controller class to the new controller class 
	PlayerControllerClass = AProjPlayerController::StaticClass(); 
}