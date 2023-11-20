// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsInCameraFOV.generated.h"


class ASonderGameState;
class AEnemyCharacter;
/**
 * 
 */
UCLASS()

// HELA SKITEN BORDE NOG VARA EN METOD I CHARACTERCAM men det får duga sålänge
class PROJ_API UBTService_IsInCameraFOV : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
			
public: /** Constructor*/
	UBTService_IsInCameraFOV();

	/** I have no idea when this is called but it is needed */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** I have no idea when this is called but it is needed as well */
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	
protected:
	/** Tick node is called every tick service is in progress*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere)
	AEnemyCharacter* OwnerCharacter = nullptr;

	FVector OwnerLocation = FVector::ZeroVector;

	ASonderGameState* SGS;
	
	float FieldOfViewAngle = 0.f;
	
};
