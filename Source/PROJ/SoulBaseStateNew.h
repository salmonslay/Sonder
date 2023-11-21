// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "SoulBaseStateNew.generated.h"

struct FInputActionInstance;
/**
 * This is the base/default state that is used by the soul character, i.e. when running around "normally" 
 */
UCLASS()
class PROJ_API USoulBaseStateNew : public UPlayerCharState
{
	GENERATED_BODY()

public:
	
	USoulBaseStateNew();
	
	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

	virtual void UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp) override;

	virtual void Exit() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bDashCoolDownActive = false;

private:
	UPROPERTY(EditAnywhere)
	class UInputAction* DashInputAction;

	UPROPERTY(EditAnywhere)
	class UInputAction* ThrowGrenadeInputAction;

	UPROPERTY(EditAnywhere)
	class UInputAction* AbilityInputAction;

	UPROPERTY()
	class AActor* LightGrenade;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> LightGrenadeRef;

	UPROPERTY()
	class ASoulCharacter* SoulCharacter;

	UPROPERTY(EditAnywhere)
	float DashCooldown = 1.f; 

	bool bHasSetUpInput = false;

	UPROPERTY(Replicated)
	float TimeHeld;

	// float 

	/** Run locally and called when player presses the dash-button */
	void Dash();

	void GetTimeHeld(const FInputActionInstance& Instance);

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerRPCThrowGrenade(const float TimeHeldGrenade);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCThrowGrenade(const float TimeHeldGrenade);
	
	/** Enables dash cooldown on server, bool is replicated to client */
	UFUNCTION(Server, Reliable)
	void ServerRPC_EnableDashCooldown();

	/** Disables dash cooldown on server, bool is replicated to client */
	UFUNCTION(Server, Reliable)
	void ServerRPC_DisableDashCooldown(); 

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ActivateAbilities();
	
};
