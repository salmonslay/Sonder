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

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// States (public for easy access to switch states)
	
	UPROPERTY(EditAnywhere)
	class URobotBaseState* RobotBaseState;

	UPROPERTY(EditAnywhere)
	class URobotHookingState* HookState; 

private:


	
	
};
