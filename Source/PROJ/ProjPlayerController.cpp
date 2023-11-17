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
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	// ask server to spawn a hero class depend on player's previous choice, and ask this server version pc to poccess
	APawn* Pawn = this->GetPawn();
	if (Pawn)
		Pawn->Destroy();

	Pawn = nullptr;

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No player starts found"));
		return;
	}

	playerSpawnPoint = PlayerStarts[0];

	APROJGameMode* Gm = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (Gm)
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		UClass* SoulClass = Gm->PlayerPawnClasses[1];
		UClass* RobotClass = Gm->PlayerPawnClasses[0];
		APROJCharacter* Hero = GetWorld()->SpawnActor<APROJCharacter>(SoulClass, playerSpawnPoint->GetTransform(), SpawnParam); // TODO: don't always spawn soul
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
