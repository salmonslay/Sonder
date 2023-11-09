// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "LevelSwitcher.generated.h"

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
	TArray<AActor*> OverlappingActors;

	// The name (path) of the level to switch to
	UPROPERTY(EditAnywhere)
	FName LevelName = "/Game/Maps/TestMaps/SofiaTestMap";
};
