// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CombatTrigger.generated.h"

class ACombatManager;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class PROJ_API ACombatTrigger : public AActor
{
	GENERATED_BODY()
public:
	ACombatTrigger();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	
	UBoxComponent* Bounds;

	UPROPERTY(BlueprintReadOnly)
	ACombatManager* Manager;
};
