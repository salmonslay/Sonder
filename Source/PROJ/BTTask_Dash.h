// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Dash.generated.h"

/**
 * Dashes immediately if enough time has passes, returns success both if Shadow Soul could dash and if it was on cooldown
 * Different behaviour might be wanted later on 
 */
UCLASS()
class PROJ_API UBTTask_Dash : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	/** How much time that needs to pass to dash again */
	UPROPERTY(EditAnywhere)
	float DashCooldown = 1.5f;

	float TimeOnLastDash = -1; 
	
};
