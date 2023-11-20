// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PROJGameMode.generated.h"

class APROJCharacter;

UCLASS(minimalapi)
class APROJGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	APROJGameMode();

	virtual void BeginPlay() override;

	APROJCharacter* GetActivePlayer(int Index) const;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual void HandleSeamlessTravelPlayer(AController*& Controller) override;

	virtual void PostSeamlessTravel() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetPlayerPointers();

	UPROPERTY(Replicated, BlueprintReadOnly)
	APROJCharacter* ServerPlayer;

	UPROPERTY(Replicated, BlueprintReadOnly)
	APROJCharacter* ClientPlayer;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<APROJCharacter>> PlayerPawnClasses; 
	
private:

	FTimerHandle PlayerPointerTimerHandle;

	TArray<class APlayerStart*> UnusedPlayerStarts;

	inline static int PlayerCount = 0;

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;
};



