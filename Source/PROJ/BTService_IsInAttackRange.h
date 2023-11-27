// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsInAttackRange.generated.h"

class AFlyingEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTService_IsInAttackRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()

				
public: /** Constructor*/
	UBTService_IsInAttackRange();

	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** I have no idea when this is called but it is needed as well */
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	
protected:
	/** Tick node is called every tick service is in progress*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY(VisibleAnywhere)
	AFlyingEnemyCharacter* OwnerCharacter = nullptr;

	FVector OwnerLocation = FVector::ZeroVector;
	
	FVector PlayerToAttackLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;
	
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries;

	bool StartAttackPointValid(const FVector& PointToCheck);

	FVector GenerateNewStartAttackPoint();
};
