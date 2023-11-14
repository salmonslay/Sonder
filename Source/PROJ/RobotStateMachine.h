// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStateMachine.h"
#include "RobotStateMachine.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API ARobotStateMachine : public ACharacterStateMachine
{
	GENERATED_BODY()

public:

	ARobotStateMachine();

	// States (public for easy access to switch states)
	
	UPROPERTY(EditAnywhere)
	class URobotBaseState* RobotBaseState;

	UPROPERTY(EditAnywhere)
	class URobotHookingState* HookState;

	// Events

	/** Run when hook is shot out, regardless if it hit Soul or an obstacle */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHookShotStart();

	/** Run when hook is fully retracted, regardless if it hit Soul or an obstacle */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHookShotEnd();

	/** Run when Robot collides with Soul */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHookExplosion(); 

private:
	
	
	
};
