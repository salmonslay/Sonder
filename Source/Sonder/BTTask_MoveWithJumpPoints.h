// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveWithJumpPoints.generated.h"

class AShadowCharacter;
class AEnemyJumpPoint;
/**
 * 
 */
UCLASS()
class SONDER_API UBTTask_MoveWithJumpPoints : public UBTTask_MoveTo
{
	GENERATED_BODY()

	
public:

	UBTTask_MoveWithJumpPoints();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	FVector GetClosestReachableJumpPointLocation();

	FVector GetClosestJumpPointTo(const FVector& Loc);

	TWeakObjectPtr<AEnemyJumpPoint> GetJumpPointFromLocation(const FVector& Loc);

	void GetJumpPointPath(const FVector& ClosestPointLoc);

	TArray<TWeakObjectPtr<AEnemyJumpPoint>> JumpPoints = TArray<TWeakObjectPtr<AEnemyJumpPoint>>();

	TArray<FVector> JumpPointLocations = TArray<FVector>();
	TArray<FVector> JumpPointPath = TArray<FVector>();
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyCurrentTarget;

	FVector OwnerLocation = FVector::ZeroVector;

	FVector CurrentTargetLocation = FVector::ZeroVector;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY()
	TWeakObjectPtr<AShadowCharacter> OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	float MaxDistanceToMarkAsReachable= 800.f;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyJumpPoint> JumpPointClass;
};
