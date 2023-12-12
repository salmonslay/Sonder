// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetValidPath.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBTService_SetValidPath : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTService_SetValidPath();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyCurrentMoveTarget;

	/** Height difference between target location and actor location to consider it unwalkable */
	UPROPERTY(EditAnywhere)
	float HeightDifferenceToMarkInvalid = 150.f;

	void SetPathIsInvalid(UBehaviorTreeComponent& OwnerComp) const; 
	
};
