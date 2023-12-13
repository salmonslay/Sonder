// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CanJumpOnPlatform.generated.h"

class AShadowCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UBTService_CanJumpOnPlatform : public UBTService_BlackboardBase
{
	GENERATED_BODY()


public:

	UBTService_CanJumpOnPlatform();

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	

private:

	UPROPERTY(VisibleAnywhere)
	AShadowCharacter* OwnerCharacter = nullptr;

	UPROPERTY(VisibleAnywhere)
	FVector OwnerLocation = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere)
	float RadiusToDetectPlatform = 200.f;

	UPROPERTY(EditAnywhere)
	float MaxJumpDistance = 50.f;

	UPROPERTY(EditAnywhere)
	float JumpBoost = 10.f;
	
	UPROPERTY(EditAnywhere)
	bool bDebug = false;
	
	/** Calculates force to jump on platform if jump is possible, and performs jump */ 
	void JumpToPoint(const FVector &StartPoint, const FVector &JumpPoint) const;

	// Currently not in use
	bool CheckPathToPlayer(const FVector &StartPoint, const FVector &CurrentTargetPoint);

	/** Check if there's a path between AI character and player character*/
	bool HasNavigationTo(const FVector &StartPoint, const FVector &CurrentTargetPoint) const ;

	float JumpCoolDownTimer = 0.f;
};
