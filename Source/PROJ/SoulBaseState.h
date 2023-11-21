// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "SoulBaseState.generated.h"

/**
 * DEPRECATED. DO NOT USE. USE SOULBASESTATENEW INSTEAD
 * 
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

	// Should not be needed but sometimes only works with it (why!?) 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly)
	bool bDashCoolDownActive = false;

private:
	UPROPERTY(EditAnywhere)
	class UInputAction* DashInputAction;

	/** Run locally and called when player presses the dash-button */
	void Dash();

	UPROPERTY()
	class ASoulCharacter* SoulCharacter;

	UPROPERTY(Replicated, EditAnywhere)
	float DashCooldown = 1.f;

	void DisableDashCooldown() { bDashCoolDownActive = false; }

	bool bHasSetUpInput = false;
};
