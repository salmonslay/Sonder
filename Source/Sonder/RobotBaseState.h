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
class SONDER_API URobotBaseState : public UPlayerCharState
{
	GENERATED_BODY()

public:
	URobotBaseState();

	virtual void UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp) override;

	void ApplySoulDashBuff();

	/** Returns the multiplier to increase damage with if buffed, will return 1 if not buffed */
	float GetDamageBoostMultiplier() const { return bHasDashBuff ? BuffedDamageMultiplier : 1; }

	UFUNCTION(BlueprintPure)
	bool IsCanPulse() const { return bCanPulse; }

	UFUNCTION(BlueprintPure)
	bool IsHookShotOnCooldown() const { return bHookShotOnCooldown; }

	UFUNCTION(BlueprintPure)
	bool IsPulseCoolDownActive() const { return bPulseCoolDownActive; }

	void Pulse();
	
	UPROPERTY()
	AController* Controller;

	UPROPERTY(EditAnywhere)
	float Damage = 5.0f;

protected:
	
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* HookShotInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* PulseInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* AbilityInputAction;

	bool bCanPulse = true;

	UPROPERTY()
	ACharacter* PlayerActor;

	UPROPERTY()
	class ARobotStateMachine* RobotCharacter;

	UPROPERTY()
	class AShadowRobotCharacter* ShadowRobot; 

	/** Time needed between hook shots */
	UPROPERTY(EditAnywhere)
	float HookShotCooldownDelay = 2.f;

	UPROPERTY(Replicated)
	bool bHookShotOnCooldown = false;

	/** Pulse Cooldown */
	UPROPERTY(Replicated)
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

	UPROPERTY()
	class UCharacterMovementComponent* MovementComponent;

	UPROPERTY()
	class USphereComponent* PulseCollision;
	
	UPROPERTY(EditAnywhere)
	float PulseCooldown = 1.f;

	UPROPERTY(EditAnywhere)
	float PulseCooldownInArena = 1.8f; 

	/** Function firing when player presses button to request hook shot */
	void ShootHook();

	UFUNCTION(Server, Reliable)
	void ServerRPCPulse();

	/** Pulse function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPulse();

	void DisableHookShotCooldown() { bHookShotOnCooldown = false; };

	UFUNCTION(Server, Reliable)
	void ServerRPC_DisablePulseCooldown(); 

	void DisableSecondJump() { if (CharOwner->IsPlayerControlled()) {PlayerActor->JumpMaxCount = 1; }}

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

	void SetPulseCooldown(); 
	
};
