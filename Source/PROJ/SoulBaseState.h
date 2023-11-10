// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "SoulBaseState.generated.h"

/**
 * This is the base/default state that is used by the soul character, i.e. when running around "normally" 
 */
UCLASS()
class PROJ_API USoulBaseState : public UPlayerCharState
{
	GENERATED_BODY()

public:

	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

	virtual void UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp) override;

	virtual void Exit() override;

private:

	UPROPERTY(EditAnywhere)
	class UInputAction* DashInputAction;

	/** Run locally and called when player presses the dash-button */ 
	void Dash();

	/** Run only on server, will tell all players that a player dashed */
	UFUNCTION(Server, Reliable)
	void ServerRPCDash();

	/** Run for every player, server and client */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDash();

	UPROPERTY()
	class ASoulCharacter* SoulCharacter;
};
