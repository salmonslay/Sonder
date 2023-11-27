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
class PROJ_API UBTService_CanAttackPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()

			
public: /** Constructor*/
	UBTService_CanAttackPlayer();

	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** I have no idea when this is called but it is needed as well */
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	
protected:
	/** Tick node is called every tick service is in progress*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY(VisibleAnywhere)
	AEnemyCharacter* OwnerCharacter = nullptr;


	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RadiusToMoveTowardsAttackingPlayer = 0.f;


	UPROPERTY(EditAnywhere)
	bool bDebug = false;
	
};
