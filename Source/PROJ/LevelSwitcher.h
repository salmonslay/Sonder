// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CutsceneManager.h"
#include "Engine/TriggerBox.h"
#include "LevelSwitcher.generated.h"

enum class ESonderLevel : uint8;
/**
 * 
 */
UCLASS()
class PROJ_API ALevelSwitcher : public ATriggerBox
{
	GENERATED_BODY()

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	// Whether or not we're switching scene
	bool bIsSwitching = false;

	// An array of actors currently overlapping this trigger
	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	// The level to switch to
	UPROPERTY(EditAnywhere)
	ESonderLevel LevelToLoad = ESonderLevel::MainMenu;

};
