// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PROJGameMode.generated.h"

class APROJCharacter;

UCLASS(minimalapi)
class APROJGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	APROJGameMode();

	APROJCharacter* GetActivePlayer(int Index);


	virtual void BeginPlay() override;


};



