// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Patrol.generated.h"

class AEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTTask_Patrol : public UBTTask_BlackboardBase
{
	GENERATED_BODY()


	/** Constructor*/
	UBTTask_Patrol();
	
	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** Tick task is called every tick this task is in progress, set up in execute task */
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY()
	AEnemyCharacter* OwnerCharacter;

	FVector OwnerCharacterLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UPROPERTY(EditAnywhere)
	float DistanceToCheck = 0.f;

	UPROPERTY(EditAnywhere)
	float DownDistanceToCheck = 0.f;

	void TurnAround();
	
};
