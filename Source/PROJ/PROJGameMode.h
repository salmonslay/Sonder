// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PROJGameMode.generated.h"

class APROJCharacter;

UCLASS(minimalapi)
class APROJGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APROJGameMode();

	APROJCharacter* GetActivePlayer(int Index) const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

private:
	TArray<class APlayerStart*> UnusedPlayerStarts;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<APROJCharacter>> PlayerPawnClasses;

	inline static int PlayerCount = 0;
};
