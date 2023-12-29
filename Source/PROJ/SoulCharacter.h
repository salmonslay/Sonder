// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStateMachine.h"
#include "Components/SphereComponent.h"
#include "SoulCharacter.generated.h"

/**
 * The character class for the Soul character, acts as a state machine 
 */
UCLASS()
class PROJ_API ASoulCharacter : public ACharacterStateMachine
{
	GENERATED_BODY()

public:
	ASoulCharacter();

	// states are public so switch state can be called with the appropriate state easily 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoulBaseStateNew* BaseStateNew;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoulDashingState* DashingState;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USphereComponent* ThrowLoc;

#pragma region Events

	UFUNCTION(BlueprintImplementableEvent)
	void OnDash();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDashEnd();

	/** Called when Soul starts charging the grenade */
	UFUNCTION(BlueprintImplementableEvent)
	void OnGrenadeThrowStart();
	
	/** Called when Soul throws the grenade */
	UFUNCTION(BlueprintImplementableEvent)
	void OnGrenadeThrowEnd();

#pragma endregion

};
