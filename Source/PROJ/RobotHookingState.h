// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "RobotHookingState.generated.h"

/**
 * State when the Robot requests to Hook 
 */
UCLASS()
class PROJ_API URobotHookingState : public UPlayerCharState
{
	GENERATED_BODY()

public:

	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

private:

	
	
};
