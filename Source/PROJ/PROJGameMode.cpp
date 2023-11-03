// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROJGameMode.h"
#include "PROJCharacter.h"
#include "Camera/CameraActor.h"
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
}

void APROJGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetUpCamera(); 
	
}

void APROJGameMode::SetUpCamera()
{
	// if(!HasAuthority()
	// 	return;

	// UE_LOG(LogTemp, Warning, TEXT("game mode hello"))

	// Get the camera in the level 
	const auto MainCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass())); 

	// Set it to be used for every player 
	for(int i = 0; i < UGameplayStatics::GetNumPlayerControllers(this); i++) 
			UGameplayStatics::GetPlayerController(this, i)->SetViewTarget(MainCamera); 
}
