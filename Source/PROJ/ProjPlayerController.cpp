// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjPlayerController.h"

#include "PROJCharacter.h"
#include "PROJGameMode.h"
#include "SoulCharacter.h"
#include "Camera/CameraActor.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
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
	if (GetViewTarget() != MainCam)
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

void AProjPlayerController::OnFinishSeamlessTravel()
{
	FInputModeGameOnly inputMode;
	SetInputMode(inputMode);

	// ask server to spawn a hero class depend on player's previous choice, and ask this server version pc to poccess
	APawn* pawn = this->GetPawn();
	if (pawn)
		pawn->Destroy();

	pawn = nullptr;

	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);

	if (playerStarts.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Please a playerStart in GamePlay map,  AGamePlayGM::SpawnPlayer"))
		return;
	}

	playerSpawnPoint = playerStarts[0];

	APROJGameMode* gm = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gm)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		UClass* SoulClass = gm->PlayerPawnClasses[1];
		UClass* RobotClass = gm->PlayerPawnClasses[0];
		APROJCharacter* Hero = GetWorld()->SpawnActor<APROJCharacter>(SoulClass, playerSpawnPoint->GetTransform(), spawnParam);
		this->Possess(Hero);
	}
}

void AProjPlayerController::SetCamera()
{
	if (!MainCam)
		MainCam = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));

	if (IsValid(MainCam))
		SetViewTarget(MainCam);
}
