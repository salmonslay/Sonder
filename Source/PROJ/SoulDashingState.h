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

	UPROPERTY(EditAnywhere)
	float DashForce = 180000.f; 

	/** The impulse/force need to be applied on the server */
	UFUNCTION(Server, Reliable)
	void ServerRPCDash(const FVector DashDir); 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDash();

	UFUNCTION(Server, Reliable)
	void ServerExit(const FVector InputVec); 

	float TempTimer = 0;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> DashBarCollisionChannel;

	UPROPERTY(EditAnywhere)
	float MaxDashDistance = 250.f;

	FVector StartLoc;
	
};
