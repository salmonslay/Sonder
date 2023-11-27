// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "SoulCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RobotBaseState.generated.h"

/**
 * The Robot's base state 
 */
UCLASS()
class PROJ_API URobotBaseState : public UPlayerCharState
{
	GENERATED_BODY()

public:
	URobotBaseState();

	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

	virtual void UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp) override;

	virtual void Exit() override;

	void ApplySoulDashBuff();

	/** Returns the multiplier to increase damage with if buffed, will return 1 if not buffed */
	float GetDamageBoostMultiplier() const { return bHasDashBuff ? BuffedDamageMultiplier : 1; }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* HookShotInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* PulseInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* AbilityInputAction;


	bool bHasSetUpInput = false;

	bool bCanPulse = true;

	ACharacter* PlayerActor;

	UPROPERTY()
	class ARobotStateMachine* RobotCharacter;

	/** Time needed between hook shots */
	UPROPERTY(EditAnywhere)
	float HookShotCooldownDelay = 2.f;

	bool bHookShotOnCooldown = false;

public:
	UFUNCTION(BlueprintPure)
	bool IsCanPulse() const { return bCanPulse; }

	UFUNCTION(BlueprintPure)
	bool IsHookShotOnCooldown() const { return bHookShotOnCooldown; }

	UFUNCTION(BlueprintPure)
	bool IsPulseCoolDownActive() const { return bPulseCoolDownActive; }

	UPROPERTY()
	AController* Controller;

	UPROPERTY(EditAnywhere)
	float Damage = 5.0f;

private:
	UPROPERTY(EditAnywhere)
	float PulseCooldown = 1.f;

	/** Pulse Cooldown */
	bool bPulseCoolDownActive = false;

	/** How fast to make Robot upon receiving the Soul dash buff */
	UPROPERTY(EditAnywhere)
	float WalkSpeedWhenBuffed = 750.f;

	/** Used to reset walk speed when buff ends, is set in BeginPlay */
	float DefaultWalkSpeed = 500.f;

	/** How long the buff should last in seconds */
	UPROPERTY(EditAnywhere, meta=(Units="seconds"))
	float DashBuffLength = 5.f;

	FTimerHandle BuffTimerHandle;

	/** If currently buffed */
	UPROPERTY(Replicated)
	bool bHasDashBuff = false;

	/** Multiplier to damage when Robot has been buffed by Soul's dash */
	UPROPERTY(EditAnywhere)
	float BuffedDamageMultiplier = 2.f;

	/** Function firing when player presses button to request hook shot */
	void ShootHook();

	void Pulse();

	UFUNCTION(Server, Reliable)
	void ServerRPCPulse();

	/** Pulse function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPulse();

	void DisableHookShotCooldown() { bHookShotOnCooldown = false; };

	void DisablePulseCooldown() { bPulseCoolDownActive = false; }

	void DisableSecondJump() { PlayerActor->JumpMaxCount = 1; }

	void ResetDashBuff();

	UFUNCTION(Server, Reliable)
	void ServerRPC_DashBuffStart();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DashBuffStart();

	UFUNCTION(Server, Reliable)
	void ServerRPC_DashBuffEnd();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DashBuffEnd();

	void ActivateAbilities();
};
