// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjPlayerController.h"

#include "PROJCharacter.h"
#include "Camera/CameraActor.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AProjPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetCamera();

	PlayerCount++; 
}

void AProjPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	PlayerCount--; 
}

void AProjPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO: Figure out why the player camera changes automatically after a few seconds so this can be removed 
	if(GetViewTarget() != MainCam) 
		SetViewTarget(MainCam); 
}

void AProjPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APROJCharacter* MyPlayerCharacter = Cast<APROJCharacter>(InPawn);

	if (MyPlayerCharacter && IsValid(MyPlayerCharacter))
	{
		PlayerCharacter = MyPlayerCharacter;
		PlayerCharacter->Restart();
		UE_LOG(LogTemp, Warning, TEXT("Restarted player pawn"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried possess pawn, failed cast"));
	}
	
}

void AProjPlayerController::SetCamera()
{
	if(!MainCam)
		MainCam = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass())); 
	
	if(IsValid(MainCam)) 
		SetViewTarget(MainCam); 
}
