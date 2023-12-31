// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_PlayerInChaseRadius.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBTService_PlayerInChaseRadius : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTService_PlayerInChaseRadius();
	
protected:
	/** Tick node is called every tick service is in progress*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere)
	ACharacter* OwnerCharacter = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RadiusToChasePlayer = 0.f;
	
	UPROPERTY(EditAnywhere)
	bool bDebug = false;
};
