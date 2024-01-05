// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
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
	MainMenu = 5, 
	Tutorial = 10,
	Level1 = 20,
	Arena1 = 30,
	Level2 = 40,
	Arena2 = 50,
	Ending = 60,
	ArenaSurvival = 70,
};

UENUM(BlueprintType)
enum class EScalability : uint8
{
	Epic = 0, 
	High = 1,
	Medium = 2, 
	Low = 3,
	Auto = 10
};

UENUM(BlueprintType)
enum class ELanguage : uint8
{
	English = 0,
	Swedish, 
};

USTRUCT(BlueprintType)
struct FLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESonderLevel Level = ESonderLevel::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText LevelTitle = FText::FromString("None");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* LevelCover = nullptr;
};

UCLASS()
class PROJ_API USonderSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<ESonderLevel> LevelsCompleted = {ESonderLevel::None};

	UFUNCTION(BlueprintCallable)
	void AddLevelCompleted(const ESonderLevel LevelCompleted) { LevelsCompleted.AddUnique(LevelCompleted); }

	UFUNCTION(BlueprintPure)
	TArray<ESonderLevel> GetLevelsCompleted() const { return LevelsCompleted; }

	UFUNCTION(BlueprintCallable)
	void ResetMapProgress() { LevelsCompleted = {ESonderLevel::None}; } 

	/**
	 * Iter through all completed levels and return the highest one
	 * @return The highest level completed, or ESonderLevel::None if no levels have been completed
	 */
	UFUNCTION(BlueprintPure)
	ESonderLevel GetHighestLevelCompleted() const;

	/**
	 * @return A map of all the level enums and their file paths
	 */
	UFUNCTION(BlueprintPure)
	static TMap<ESonderLevel, FString> GetLevelPaths();

	/**
	 * Calculate which level to travel to based on what you've completed
	 * @param From The level you're coming from (i.e. the level you just completed, or the highest level you've completed if you're continuing)
	 * @return The path to the level you should travel to
	 */
	UFUNCTION(BlueprintPure)
	static FString GetLevelToContinueTo(const ESonderLevel From = ESonderLevel::None);
	
	UFUNCTION(BlueprintPure)
	static bool CanPlayLevel(const ESonderLevel LevelToPlay);

	UPROPERTY(BlueprintReadWrite)
	TArray<FEnhancedActionKeyMapping> InputMappings;

	/** Brightness set through settings */
	UPROPERTY(BlueprintReadWrite)
	float Brightness = 0;

	UPROPERTY(BlueprintReadWrite)
	float MasterVolume = 0.5f;

	UPROPERTY(BlueprintReadWrite)
	EScalability CurrentScalability = EScalability::High;

	UPROPERTY(BlueprintReadWrite)
	ELanguage CurrentLanguage = ELanguage::English; 
	
};
