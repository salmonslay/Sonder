// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PROJCharacter.h"
#include "CharacterStateMachine.generated.h"

class UPlayerCharState; 

/**
 * This is the state machine for the player 
 */

UCLASS()
class PROJ_API ACharacterStateMachine : public APROJCharacter 
{
	GENERATED_BODY()

public:

	ACharacterStateMachine(); 

	/** Switches the current state to the new one */
	void SwitchState(UPlayerCharState* NewState);

protected:

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	void UpdateStateInputComp() const; 

	/** Override without calling Super and return the wanted starting state */
	virtual UPlayerCharState* GetStartingState() const; 
	
	// The player's current state
	UPROPERTY()
	UPlayerCharState* CurrentState = nullptr;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:

	/** This is an example of how a state can be added and created in the constructor, will be removed */
	UPROPERTY(EditAnywhere)
	class UDummyPlayerState* DummyState;
	
};
