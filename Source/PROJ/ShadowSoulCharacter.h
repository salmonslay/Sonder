// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShadowCharacter.h"
#include "ShadowSoulCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AShadowSoulCharacter : public AShadowCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShadowSoulCharacter();

	UPROPERTY(EditAnywhere)
	class USoulBaseStateNew* SoulBaseState;

	UPROPERTY(EditAnywhere)
	class USoulDashingState* DashState; 

	// EVENTS

	UFUNCTION(BlueprintImplementableEvent)
	void OnDash();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDashEnd();

protected:

	virtual void BeginPlay() override;
	
};
