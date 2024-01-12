// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CanAttackPlayer.generated.h"

class AEnemyCharacter;
class APROJCharacter;
/**
 * 
 */
UCLASS()
class SONDER_API UBTService_CanAttackPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
			
public: /** Constructor*/
	UBTService_CanAttackPlayer();
	
protected:
	/** Tick node is called every tick service is in progress*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere)
	AEnemyCharacter* OwnerCharacter = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RadiusToDetectPlayer = 0.f;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;
	
};
