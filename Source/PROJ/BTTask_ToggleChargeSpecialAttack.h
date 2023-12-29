// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ToggleChargeSpecialAttack.generated.h"

/**
 * Toggles charging the special attack 
 */
UCLASS()
class PROJ_API UBTTask_ToggleChargeSpecialAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTTask_ToggleChargeSpecialAttack(); 

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Execute task is called once every time this task is activated */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyChargingSpecialAttack; 
	
	UPROPERTY(EditAnywhere)
	bool bActivateCharge = true;

	UPROPERTY()
	class AShadowCharacter* ShadowOwner; 
	
};
