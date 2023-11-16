// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "SoulBaseStateNew.generated.h"

/**
 * This is the base/default state that is used by the soul character, i.e. when running around "normally" 
 */
UCLASS()
class PROJ_API USoulBaseStateNew : public UPlayerCharState
{
	GENERATED_BODY()

public:

	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

	virtual void UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp) override;

	virtual void Exit() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(EditAnywhere)
	class UInputAction* DashInputAction;

	/** Run locally and called when player presses the dash-button */ 
	void Dash();

	UPROPERTY()
	class ASoulCharacter* SoulCharacter;

	UPROPERTY(EditAnywhere)
	float DashCooldown = 1.f; 

	bool bDashCoolDownActive = false;

	void DisableDashCooldown() { bDashCoolDownActive = false; }
	
	bool bHasSetUpInput = false; 
	
};
