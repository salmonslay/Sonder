// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChargeLaserAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBTTask_ChargeLaserAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	/** Constructor*/
	UBTTask_ChargeLaserAttack();
	
	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** Tick task is called every tick this task is in progress, set up in execute task */
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY()
	class AFlyingEnemyCharacter* OwnerCharacter;

	FVector OwnerLocation;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;


	FTimerHandle EnableMovementTimerHandle;

	bool bIsCharging = false;

	void EnableMovement();
	
};
