// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PROJGameMode.generated.h"

UCLASS(minimalapi)
class APROJGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	APROJGameMode();

protected:

	virtual void BeginPlay() override;

private:

	void SetUpCamera(); 
	
};



