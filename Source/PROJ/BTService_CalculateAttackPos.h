// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CalculateAttackPos.generated.h"

class AFlyingEnemyCharacter;

/**
 * 
 */
UCLASS()
class PROJ_API UBTService_CalculateAttackPos : public UBTService_BlackboardBase
{
	GENERATED_BODY()


				
public: /** Constructor*/
	UBTService_CalculateAttackPos();

	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** I have no idea when this is called but it is needed as well */
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	
protected:
	/** Tick node is called every tick service is in progress*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool IsStartAttackPositionValid(const FVector& PosToCheck);

	UPROPERTY(VisibleAnywhere)
	AFlyingEnemyCharacter* OwnerCharacter = nullptr;

	FVector OwnerLocation = FVector::ZeroVector;

	FVector PlayerToAttackPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;
};
