// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SonderGameState.generated.h"


/**
 * 
 */

UENUM()
enum EDifficulty
{
	Difficulty_Easy			UMETA(DisplayName = "Easy"),
	Difficulty_Medium		UMETA(DisplayName = "Medium"),
	Difficulty_Hard			UMETA(DisplayName = "Hard"),
};

class APROJCharacter;
UCLASS()
class PROJ_API ASonderGameState : public AGameState
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	EDifficulty GetDifficulty();

	UFUNCTION(BlueprintCallable)
	void SetDifficulty(EDifficulty DiffToSet);
	
	bool BothPlayersAreSet();

	UFUNCTION(BlueprintPure)
	APROJCharacter* GetServerPlayer();
	
	UFUNCTION(BlueprintPure)
	APROJCharacter* GetClientPlayer();

	bool IsInCameraFieldOfView(const FVector& Location) const;

private:
	
	EDifficulty Difficulty;
};
