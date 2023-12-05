// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_DoBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBTTask_DoBasicAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTTask_DoBasicAttack(); 

	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
