// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToMovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBTTask_MoveToMovingPlatform : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:

	UBTTask_MoveToMovingPlatform();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FVector GetMovingPlatform (const UBehaviorTreeComponent& OwnerComp);

	UPROPERTY()
	TArray<FVector> MovingPlatforms;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyCurrentTarget;

	FVector OwnerLocation = FVector::ZeroVector;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditAnywhere)
	float MaxHeightDifferenceToMarkAsSameHeight = 100.f;
	
};
