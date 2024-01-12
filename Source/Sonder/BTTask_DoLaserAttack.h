// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_DoLaserAttack.generated.h"

class AFlyingEnemyCharacter;
/**
 * 
 */
UCLASS()
class SONDER_API UBTTask_DoLaserAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	/** Constructor*/
	UBTTask_DoLaserAttack();

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	

	UPROPERTY()
	AFlyingEnemyCharacter* OwnerCharacter;

	FVector PlayerToAttackLocation = FVector::ZeroVector;

	FVector OwnerLocation;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries;
	
};
