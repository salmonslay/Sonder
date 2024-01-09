// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetDashThroughRobot.generated.h"

/**
 * 
 */
UCLASS()
class SONDER_API UBTService_SetDashThroughRobot : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTService_SetDashThroughRobot(); 

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyDoBasicAttack;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyDashThroughRobot;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyDoSpecialAttack;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ShadowObjectType;

	/** How far away a Robot needs to be to dash through, assuming all other conditions are met */
	UPROPERTY(EditAnywhere)
	float DistanceToRobotToDash = 100.f; 

	/** Returns true if the Owner (Shadow Soul) is attacking, determined by seeing if any attack bool is true */
	bool IsAttacking(UBehaviorTreeComponent& OwnerComp) const;

	/** Returns true if it's appropriate to dash (resulting in a probable Robot buff) */
	bool ShouldDashThroughRobot(const APawn* Owner) const;
	
};
