// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROJGameMode.h"
#include "PROJCharacter.h"
#include "ProjPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

APROJGameMode::APROJGameMode()
{
	// This is from UE auto generation 

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default player controller class to the new controller class 
	PlayerControllerClass = AProjPlayerController::StaticClass();

	bReplicates = true;
}

void APROJGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(PlayerPointerTimerHandle, this, &APROJGameMode::SetPlayerPointers, 2, true);
}

APROJCharacter* APROJGameMode::GetActivePlayer(const int Index) const
{
	const AGameStateBase* GSB = GetGameState<AGameStateBase>();
	ensure(GSB != nullptr);

	APROJCharacter* PlayerToReturn = nullptr;
	if (GSB->PlayerArray.Num() <= Index)
	{
		return PlayerToReturn;
	}
	if (GSB->PlayerArray[Index] != nullptr)
	{
		PlayerToReturn = Cast<APROJCharacter>(GSB->PlayerArray[Index]->GetPawn());
	}
	return PlayerToReturn;
}

void APROJGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	PlayerCount = 0;
}

void APROJGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Find and store player starts 
	TArray<AActor*> TempPlayerStarts;

	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), TempPlayerStarts);

	for (const auto PlayerStartActor : TempPlayerStarts)
	{
		if (auto PlayerStart = Cast<APlayerStart>(PlayerStartActor))
			UnusedPlayerStarts.Add(PlayerStart);
	}
}

void APROJGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                             FString& ErrorMessage)
{
	if (UnusedPlayerStarts.IsEmpty())
		ErrorMessage = "Server full. No valid player starts, are there 2 in the level?";

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

FString APROJGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
                                     const FString& Options, const FString& Portal)
{
	// No unused player starts, player cannot spawn 
	if (UnusedPlayerStarts.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No unused player starts in InitNewPlayer. Are there 2 in the level?"))
		return FString("No unused player starts. Are there 2 in the level?");
	}

	// Set the new player's spawn position 
	NewPlayerController->StartSpot = UnusedPlayerStarts.Pop();

	// Get the pawn class to use based on player count, we might want to handle this differently in the future
	TArray<TSubclassOf<APROJCharacter>> ReversedPlayerPawnClasses;
	for (int i = PlayerPawnClasses.Num() - 1; i >= 0; i--)
		ReversedPlayerPawnClasses.Add(PlayerPawnClasses[i]);
	const auto PawnClassToSpawn = ReversedPlayerPawnClasses[PlayerCount];

	DefaultPawnClass = PawnClassToSpawn;

	// Set the pawn used in the player controller 
	if (const auto PlayerController = Cast<AProjPlayerController>(NewPlayerController))
	{
		PlayerController->SetControlledPawnClass(PawnClassToSpawn);
	}

	PlayerCount++;

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

UClass* APROJGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	/* Override Functionality to get Pawn from PlayerController */
	if (const AProjPlayerController* MyController = Cast<AProjPlayerController>(InController))
	{
		return MyController->GetControlledPawnClass();
	}

	/* If we don't get the right Controller, use the Default Pawn */
	return DefaultPawnClass;
}

void APROJGameMode::HandleSeamlessTravelPlayer(AController*& Controller)
{
	Super::HandleSeamlessTravelPlayer(Controller);

	// Default behavior is to spawn new controllers and copy data
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC && PC->Player)
	{
		// We need to spawn a new PlayerController to replace the old one
		APlayerController* NewPC = SpawnPlayerController(
			PC->IsLocalPlayerController() ? ROLE_SimulatedProxy : ROLE_AutonomousProxy, PC->GetFocalLocation(),
			PC->GetControlRotation());

		if (NewPC)
		{
			PC->SeamlessTravelTo(NewPC);
			NewPC->SeamlessTravelFrom(PC);
			SwapPlayerControllers(PC, NewPC);
			PC = NewPC;
			Controller = NewPC;
		}
		else
		{
			UE_LOG(LogGameMode, Warning,
			       TEXT("HandleSeamlessTravelPlayer: Failed to spawn new PlayerController for %s (old class %s)"),
			       *PC->GetHumanReadableName(), *PC->GetClass()->GetName());
			PC->Destroy();
			return;
		}
	}

	InitSeamlessTravelPlayer(Controller);

	// Initialize hud and other player details, shared with PostLogin
	GenericPlayerInitialization(Controller);

	if (PC)
	{
		AProjPlayerController* ProjPC = Cast<AProjPlayerController>(PC);
		if (ProjPC)
		{
			ProjPC->OnFinishSeamlessTravel();
		}
	}
}

void APROJGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APROJGameMode, ServerPlayer);
	DOREPLIFETIME(APROJGameMode, ClientPlayer);
}

void APROJGameMode::SetPlayerPointers()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ServerPlayer = GetActivePlayer(0);
		ClientPlayer = GetActivePlayer(1);
	}
	if (ServerPlayer && ClientPlayer)
	{
		GetWorldTimerManager().ClearTimer(PlayerPointerTimerHandle);
	}
}
