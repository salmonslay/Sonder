// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SeparateFromCollision.generated.h"

class AFlyingEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTService_SeparateFromCollision : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTService_SeparateFromCollision();
	
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

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UPROPERTY(EditAnywhere)
	float CollisionCheckRadius = 100.f;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries;

	FCollisionObjectQueryParams CollisionObjectParams;

	int CollisionCounter = 0;

	FVector TotalSeparationVector = FVector::ZeroVector;

	FVector CalculateTotalSeparationForce( FVector &Separation) const;
};
