// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetCurrentMoveTowardsTarget.generated.h"

class AEnemyJumpPoint;
class AShadowCharacter;
/**
 * 
 */
UCLASS()
class SONDER_API UBTService_SetCurrentMoveTowardsTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

	
public:

	UBTService_SetCurrentMoveTowardsTarget();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	FVector GetClosestReachableJumpPointLocation();

	FVector GetClosestJumpPointTo(const FVector& Loc);

	TWeakObjectPtr<AEnemyJumpPoint> GetJumpPointFromLocation(const FVector& Loc);

	TArray<TWeakObjectPtr<AEnemyJumpPoint>> JumpPoints = TArray<TWeakObjectPtr<AEnemyJumpPoint>>();

	TArray<FVector> JumpPointLocations = TArray<FVector>();
	TArray<FVector> JumpPointPath = TArray<FVector>();
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyClosestMoveTarget;

	FVector OwnerLocation = FVector::ZeroVector;

	FVector CurrentTargetLocation = FVector::ZeroVector;

	FVector ClosestMoveLoc = FVector::ZeroVector;

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

#define REALLY_LARGE_NUMBER 6000000000.f
	
};
