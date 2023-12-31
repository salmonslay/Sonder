// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Dash.generated.h"

/**
 * Calls Dash in SoulBaseState immediately 
 */
UCLASS()
class PROJ_API UBTTask_Dash : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY()
	class AShadowSoulCharacter* ShadowSoul;

	UPROPERTY()
	class USoulBaseStateNew* SoulBaseState; 
	
};
