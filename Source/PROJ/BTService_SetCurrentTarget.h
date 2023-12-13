// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetCurrentTarget.generated.h"

class AShadowCharacter;
/**
 * Sets the current target to the closest player which is passed as the blackboard key 
 */
UCLASS()
class PROJ_API UBTService_SetCurrentTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override; 

private:

	AShadowCharacter* OwnerCharacter = nullptr;

	/** Returns the closest player's location */ 
	FVector GetTargetLocation(AAIController* BaseAIController);

	bool HasLineOfSightToPlayer(AShadowCharacter* Owner, class APROJCharacter* PlayerTarget) const;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> LineTraceObjects; 
	
};
