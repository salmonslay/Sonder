// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class PROJ_API AProjPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override; 
	
private:

	/** Finds the camera in the level and sets it as view target/the player's camera */
	void SetCamera(); 
	
};
