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
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPulse();

	/** Run when hook is shot out, regardless if it hit Soul or an obstacle */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHookShotStart();

	/** Run when hook is fully retracted, regardless if it hit Soul or an obstacle */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHookShotEnd();

	/** Run when Robot collides with Soul */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHookExplosion();

	/** Run when Robot receives a buff by Soul's dash */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDashBuffStart(); 

	/** Run when Robot's buff by Soul's dash ends */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDashBuffEnd(); 
	
private:
	
	
	
};
