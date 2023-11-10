// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStateMachine.h"
#include "SoulCharacter.generated.h"

/**
 * The character class for the Soul character, acts as a state machine 
 */
UCLASS()
class PROJ_API ASoulCharacter : public ACharacterStateMachine
{
	GENERATED_BODY()

public:

	ASoulCharacter(); 

	// states are public so switch state can be called with the appropriate state easily 
	
	UPROPERTY(EditAnywhere)
	class USoulBaseState* BaseState;

	UPROPERTY(EditAnywhere)
	class USoulDashingState* DashingState;

#pragma region Events

	UFUNCTION(BlueprintImplementableEvent)
	void OnDash(); 

#pragma endregion 
	
protected:

	virtual UPlayerCharState* GetStartingState() const override;
	
private:

	
	
};
