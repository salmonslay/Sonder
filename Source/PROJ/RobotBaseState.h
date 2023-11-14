// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "RobotBaseState.generated.h"

/**
 * The Robot's base state 
 */
UCLASS()
class PROJ_API URobotBaseState : public UPlayerCharState
{
	GENERATED_BODY()

public:

	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

	virtual void UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp) override;

	virtual void Exit() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* HookShotInputAction;
	
	bool bHasSetUpInput = false;

	/** Function firing when player presses button to request hook shot */
	void ShootHook();

	UPROPERTY()
	class ARobotStateMachine* RobotCharacter;

	/** Time needed between hook shots */
	UPROPERTY(EditAnywhere)
	float HookShotCooldownDelay = 2.f; 

	bool bHookShotOnCooldown = false;

	void DisableHookShotCooldown() { bHookShotOnCooldown = false; }; 
};
