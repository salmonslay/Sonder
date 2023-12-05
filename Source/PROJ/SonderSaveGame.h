// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SonderSaveGame.generated.h"

// Enum values are 10-step increments, so that we can add more levels in between if needed while maintaining the order
UENUM(BlueprintType)
enum class ESonderLevel : uint8
{
	/**
	 * None is mostly meant as a placeholder for when you change level but don't want to count it
	 */
	None = 0,
	Tutorial = 10,
	Level1 = 20,
	Arena1 = 30,
	Level2 = 40,
	Arena2 = 50,
	Ending = 60,
};

UCLASS()
class PROJ_API USonderSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<ESonderLevel> LevelsCompleted = {ESonderLevel::None};

public:
	UFUNCTION(BlueprintCallable)
	void AddLevelCompleted(const ESonderLevel LevelCompleted) { LevelsCompleted.AddUnique(LevelCompleted); }

	UFUNCTION(BlueprintPure)
	TArray<ESonderLevel> GetLevelsCompleted() const { return LevelsCompleted; }
};
