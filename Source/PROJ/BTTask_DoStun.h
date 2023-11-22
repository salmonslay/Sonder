// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_DoStun.generated.h"

class AFlyingEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTTask_DoStun : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_DoStun();
	
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** Tick task is called every tick this task is in progress, set up in execute task */
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY()
	AFlyingEnemyCharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;


	float Timer = 0.f;
};
