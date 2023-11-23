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

	UFUNCTION(BlueprintPure)
	UPlayerCharState* GetCurrentState() const { return CurrentState; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	void UpdateStateInputComp(); 

	/** Returns the base state for the appropriate character */
	UPlayerCharState* GetStartingState() const; 
	
	// The player's current state - is replicated. NOTE: The state's Enter, Update, Exit etc. is only called locally 
	UPROPERTY(Replicated)
	UPlayerCharState* CurrentState = nullptr;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:

	UFUNCTION(Server, Reliable) 
	void ServerRPC_SwitchState(UPlayerCharState* NewState); 

	/** This is an example of how a state can be added and created in the constructor, will be removed */
	UPROPERTY(EditAnywhere)
	class UDummyPlayerState* DummyState;
	
};
