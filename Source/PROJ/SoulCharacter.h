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

protected:

	virtual UPlayerCharState* GetStartingState() const override;
	
private:

	UPROPERTY(EditAnywhere)
	class USoulBaseState* BaseState; 
	
};
