// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatTriggeredBase.generated.h"

UCLASS()
class SONDER_API ACombatTriggeredBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACombatTriggeredBase();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TriggeredEvent();
};
