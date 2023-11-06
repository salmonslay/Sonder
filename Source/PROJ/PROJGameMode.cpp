// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROJGameMode.h"
#include "PROJCharacter.h"
#include "ProjPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
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

APROJCharacter* APROJGameMode::GetActivePlayer(int Index)
{
	AGameStateBase* GSB = GetGameState<AGameStateBase>();
	APROJCharacter* PlayerToReturn =nullptr;

	if (GSB->PlayerArray[Index] != nullptr)
	{
		PlayerToReturn =  Cast<APROJCharacter>(GSB->PlayerArray[Index]->GetPawn());
	}
	return PlayerToReturn;

}


void APROJGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}
