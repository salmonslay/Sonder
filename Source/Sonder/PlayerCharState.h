// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharState.generated.h"

/**
 * The base class that all player states should/must inherit from 
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SONDER_API UPlayerCharState : public UActorComponent
{
	GENERATED_BODY()

public:

	UPlayerCharState(); 

	/** Called when player transitions into this state, not safe to update the input component in this state because of
	 *  race conditions */
	virtual void Enter();  

	/** Function where it's safe to update the player's input component. Called approx. at the same time as Enter() */ 
	virtual void UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp);

	/** Called each frame that the state is active */
	virtual void Update(const float DeltaTime) {} 

	/** Called when the state exits to transition to a different state */
	virtual void Exit() {}

protected:

	/** The player's input component, useful for handling input in the different states */
	UPROPERTY()
	UEnhancedInputComponent* PlayerInputComponent;

	/** The owning player */
	UPROPERTY()
	ACharacter* CharOwner; 

	virtual void BeginPlay() override;
		
};
