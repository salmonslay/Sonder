// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShadowCharacter.h"
#include "ShadowRobotCharacter.generated.h"

UCLASS()
class PROJ_API AShadowRobotCharacter : public AShadowCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShadowRobotCharacter();

	UPROPERTY(EditAnywhere)
	class URobotBaseState* RobotBaseState;
	
	UPROPERTY(EditAnywhere)
	class URobotHookingState* RobotHookState;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPulse();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDashBuffStart();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDashBuffEnd();
	
};
