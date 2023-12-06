// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetDashThroughRobot.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBTService_SetDashThroughRobot : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTService_SetDashThroughRobot(); 

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyDoBasicAttack;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyDoSpecialAttack;

	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> ShadowObjectType;

	/** Returns true if the Owner (Shadow Soul) is attacking, determined by seeing if any attack bool is true */
	bool IsAttacking(UBehaviorTreeComponent& OwnerComp) const;

	/** Returns true if it's appropriate to dash (resulting in a probable Robot buff) */
	bool ShouldDashThroughRobot(const APawn* Owner) const;

	bool IsRobotInMovementDirection(const AActor* Robot, const APawn* Owner) const; 
	
};
