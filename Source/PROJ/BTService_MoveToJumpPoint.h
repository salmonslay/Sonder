// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTService_MoveToJumpPoint.generated.h"

class UBoxComponent;
/**
 * THIS IS A TASK! NAMED IT WRONG whoopsie  
 */
UCLASS()
class PROJ_API UBTService_MoveToJumpPoint : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:

	UBTService_MoveToJumpPoint(); 

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	FVector GetJumpPoint(const UBehaviorTreeComponent& OwnerComp);

	float MaxHeightDiff = 150.f;

	UPROPERTY()
	TArray<FVector> JumpPoints;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyCurrentTarget;

	FVector OwnerLocation = FVector::ZeroVector;
	
};
