// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveDirectlyToward.h"
#include "BTTask_MoveIdle.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBTTask_MoveIdle : public UBTTask_MoveDirectlyToward
{
	GENERATED_BODY()

public:

	UBTTask_MoveIdle(); 

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:

	// Speed when idling 
	UPROPERTY(EditAnywhere)
	float IdleMoveSpeed = 150.f;

	// Speed to reset to when done idling 
	UPROPERTY(EditAnywhere)
	float DefaultMoveSpeed = 400.f;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyMovingIdle;

	// How far to move (max) back and fourth 
	UPROPERTY(EditAnywhere)
	float MaxMoveDistance = 250.f;

	void SetTargetLocInit(UBehaviorTreeComponent& OwnerComp) const;

	FVector StartLoc; 
	
};
