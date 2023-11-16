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

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* PulseInputAction;
	
	bool bHasSetUpInput = false;
	
	bool bCanPulse = true;

	/** Function firing when player presses button to request hook shot */
	void ShootHook();

	void Pulse();

	UFUNCTION(Server, Reliable)
	void ServerRPCPulse(); 

	/** Pulse function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPulse();

	UPROPERTY()
	class ARobotStateMachine* RobotCharacter;

	/** Time needed between hook shots */
	UPROPERTY(EditAnywhere)
	float HookShotCooldownDelay = 2.f; 

	bool bHookShotOnCooldown = false;

	void DisableHookShotCooldown() { bHookShotOnCooldown = false; }; 

	UPROPERTY(EditAnywhere)
	float PulseCooldown = 1.f; 

	bool bPulseCoolDownActive = false;

	void DisablePulseCooldown() { bPulseCoolDownActive = false; }
	
};
