// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "DummyPlayerState.generated.h"

/**
 * This is just a dummy state to serve as a placeholder until we have actual states 
 */

UCLASS()
class PROJ_API UDummyPlayerState : public UPlayerCharState
{
	GENERATED_BODY()

public:

	virtual void Enter() override;
	
	virtual void Update(const float DeltaTime) override;

	virtual void Exit() override;

private:

	UPROPERTY(EditAnywhere)
	float DummyValue = 1.f; 
	
};
