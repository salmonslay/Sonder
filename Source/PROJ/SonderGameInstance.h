// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "SonderSaveGame.h"
#include "SonderGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API USonderGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool bIsPlayingLocal = false;
};
