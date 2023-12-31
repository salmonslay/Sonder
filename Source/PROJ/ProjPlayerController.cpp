// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjPlayerController.h"

#include "CharactersCamera.h"
#include "CutsceneManager.h"
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
	if (!ACutsceneManager::IsCutscenePlaying() && GetViewTarget() != MainCam)
		SetViewTarget(MainCam);
}

void AProjPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APROJCharacter* MyPlayerCharacter = Cast<APROJCharacter>(InPawn);

	if (IsValid(MyPlayerCharacter))
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
	APawn* Pawn2 = this->GetPawn();
	if (Pawn2)
		Pawn2->Destroy();

	Pawn2 = nullptr;

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No player starts found"));
		return;
	}

	playerSpawnPoint = PlayerStarts[0]; // the first checkpoint is a fallback

	const APROJGameMode* Gm = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (Gm)
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		UClass* RobotClass = Gm->PlayerPawnClasses[0];
		UClass* SoulClass = Gm->PlayerPawnClasses[1];

		UClass* PickedClass = UGameplayStatics::GetActorOfClass(GetWorld(), SoulClass) ? RobotClass : SoulClass;
		const FName ClassTag = PickedClass == SoulClass ? FName("Soul") : FName("Robot");

		// iter through all playerstarts
		TArray<AActor*> AllPlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStarts);
		for (const auto& PlayerStart : AllPlayerStarts)
		{
			if (PlayerStart->ActorHasTag(ClassTag))
			{
				playerSpawnPoint = PlayerStart;
				UE_LOG(LogTemp, Warning, TEXT("Using player start override with tag %s"), *ClassTag.ToString());
				break;
			}
		}

		APROJCharacter* Hero = GetWorld()->SpawnActor<APROJCharacter>(PickedClass, playerSpawnPoint->GetTransform(),
		                                                              SpawnParam);
		Possess(Hero);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No PROJGameMode found."));
	}
}

void AProjPlayerController::SetCamera()
{
	if (!MainCam)
		MainCam = Cast<ACharactersCamera>(UGameplayStatics::GetActorOfClass(this, ACharactersCamera::StaticClass()));

	if (IsValid(MainCam))
		SetViewTarget(MainCam);
}
