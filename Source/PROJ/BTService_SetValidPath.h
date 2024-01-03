// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetValidPath.generated.h"

class AMovingPlatform;
class APROJCharacter;
class AShadowCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTService_SetValidPath : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTService_SetValidPath();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY()
	AShadowCharacter* OwnerCharacter = nullptr;

	UPROPERTY()
	FVector OwnerLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBKeyCurrentMoveTarget;

	/** Height difference between target location and actor location to consider it unwalkable */
	UPROPERTY(EditAnywhere)
	float HeightDifferenceToMarkInvalid = 100.f;

	bool HasLineOfSightToPlayer(AShadowCharacter* Owner, const FVector &CurrentPlayerTarget) const;

	void SetPathIsInvalid(UBehaviorTreeComponent& OwnerComp) const;

	void VerifyGroundStatus(AShadowCharacter* Owner) const;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> LineTraceObjects;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMovingPlatform> MovingPlatformClass;
	
};
