// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "FlyingEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SONDER_API AFlyingEnemyAIController : public AEnemyAIController
{
	GENERATED_BODY()

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Initialize() override;

	void StopRepositioning();

	FTimerHandle RepositioningResetTimerHandle;
	
};
