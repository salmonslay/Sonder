// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChargeLaserAttack.generated.h"

/**
 * 
 */
UCLASS()
class SONDER_API UBTTask_ChargeLaserAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	/** Constructor*/
	UBTTask_ChargeLaserAttack();

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY()
	class AFlyingEnemyCharacter* OwnerCharacter;

	FVector OwnerLocation;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	FTimerHandle EnableMovementTimerHandle;

	bool bIsCharging = false;

	void EnableMovement();
	
};
