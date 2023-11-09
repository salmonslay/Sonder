// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "SoulDashingState.generated.h"

/**
 * Soul char enters this state when dashing 
 */
UCLASS()
class PROJ_API USoulDashingState : public UPlayerCharState
{
	GENERATED_BODY()

public:

	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

	virtual void Exit() override;

private:

	UPROPERTY()
	class UCharacterMovementComponent* CharMovementComp;

	UPROPERTY(EditAnywhere)
	float DashForce = 100.f;

	UFUNCTION(Server, Reliable)
	void ServerRPCDash(const FVector DashDir); 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDash();

	float TempTimer = 0;
	
};
