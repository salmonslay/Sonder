// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "SonderGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API USonderGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	USonderGameInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite)
	bool bIsPlayingLocal = false;

	/**
	 * Whether or not we're currently trying to join a session.
	 * If true, the main menu will open the lobby menu when the session is joined.
	 */
	UPROPERTY(BlueprintReadWrite)
	bool bIsCurrentlyJoining = false;

	UFUNCTION(BlueprintCallable)
	void AddToLog(FString TextToLog);

	/**
	 * The BP_SonderGameInstance will implement this event to check if any achievements have been unlocked.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void CheckAchievements();

private:
	FString FilePath;
};
