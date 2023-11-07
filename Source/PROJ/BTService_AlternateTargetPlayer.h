// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_AlternateTargetPlayer.generated.h"

class AEnemyAIController;
class APROJCharacter;
class AEnemyCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTService_AlternateTargetPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
		
public: /** Constructor*/
	UBTService_AlternateTargetPlayer();

	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** I have no idea when this is called but it is needed as well */
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	
protected:
	/** Tick node is called every tick service is in progress*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY()
	AEnemyCharacter* OwnerCharacter;

	UPROPERTY()
	AEnemyAIController* OwnerController;

	APROJCharacter* Player1 = nullptr;
	APROJCharacter* Player2 = nullptr;

	UPROPERTY(EditAnywhere)
	bool bDebug = false;
	

};
