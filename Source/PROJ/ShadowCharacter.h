// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "ShadowCharacter.generated.h"

class UPlayerCharState; 

/**
 * This is the state machine for the shadow versions of the characters which is almost identical to the player's
 * except for the input related functionality (there is none here) 
 */

UCLASS()
class PROJ_API AShadowCharacter : public AEnemyCharacter 
{
	GENERATED_BODY()

public:

	AShadowCharacter(); 

	/** Switches the current state to the new one */
	void SwitchState(UPlayerCharState* NewState);

	UFUNCTION(BlueprintPure)
	UPlayerCharState* GetCurrentState() const { return CurrentState; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// EVENTS

	/** Called when a basic attack is performed, regardless if it hit something */
	UFUNCTION(BlueprintImplementableEvent)
	void OnBasicAttack();

	/** Called when the basic attack actually lands a hit, and is called for every hit */
	UFUNCTION(BlueprintImplementableEvent)
	void OnBasicAttackHit(); 

protected:

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	/** Returns the base state for the appropriate character */
	UPlayerCharState* GetStartingState() const; 
	
	// The player's current state - is replicated. NOTE: The state's Enter, Update, Exit etc. is only called locally 
	UPROPERTY(Replicated)
	UPlayerCharState* CurrentState = nullptr;
	
private:

	UFUNCTION(Server, Reliable) 
	void ServerRPC_SwitchState(UPlayerCharState* NewState); 

	/** This is an example of how a state can be added and created in the constructor, will be removed */
	UPROPERTY(EditAnywhere)
	class UDummyPlayerState* DummyState;

	UPROPERTY(EditAnywhere)
	class UPlayerBasicAttack* EnemyBasicAttack; 
	
};