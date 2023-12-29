// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_StopLaserAttack.generated.h"

class AFlyingEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTTask_StopLaserAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTTask_StopLaserAttack();
	
	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY()
	AFlyingEnemyCharacter* OwnerCharacter;

	FVector PlayerToAttackLocation = FVector::ZeroVector;

	FVector OwnerLocation;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	void StopRepositioning();

	UPROPERTY()
	UBehaviorTreeComponent* TreeComponent;
	FTimerHandle StopRepositioningTimerHandle;
	
};
