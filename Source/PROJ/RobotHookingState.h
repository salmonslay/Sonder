// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "RobotHookingState.generated.h"

/**
 * State when the Robot requests to Hook 
 */
UCLASS()
class PROJ_API URobotHookingState : public UPlayerCharState
{
	GENERATED_BODY()

public:

	virtual void Enter() override;

	virtual void Update(const float DeltaTime) override;

	virtual void Exit() override;

private:

	UPROPERTY()
	AActor* SoulCharacter; 
	
	UPROPERTY()
	FVector CurrentHookTarget;

	UPROPERTY()
	class ARobotStateMachine* RobotCharacter;

	bool bTravellingTowardsTarget = false;

	/** How close the player needs to be for it to be considered as reached target */
	float ReachedTargetDistTolerance = 50.f;
	
	/** How fast to travel */
	UPROPERTY(EditAnywhere)
	float HookShotTravelSpeed = 300.f;

	float DefaultGravityScale = 1.75f;

	UPROPERTY()
	class UCharacterMovementComponent* MovementComponent;

	bool bShootingHookOutwards = false;
	
	UPROPERTY()
	class UCableComponent* HookCable;
	
	/** Returns false if there was a block */
	bool SetHookTarget();

	FHitResult DoLineTrace();

	/** Shoots out the physical hook, does not move player */
	void ShootHook();

	void StartTravelToTarget(); 

	/** Travels the player towards the set target */
	void TravelTowardsTarget(const float DeltaTime);
	
	/** Called at the end of a hook shot IF the Robot collided with Soul */
	void CollidedWithSoul();

	/** Run on server when Robot collides with soul at the end of the hook shot */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookCollision();

	/** Run for everyone when Robot collides with soul at the end of the hook shot */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookCollision();

	void RetractHook(const float DeltaTime) const;

	/** Run on server when hook is shot regardless if it hits Soul or an obstacle */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookShotStart();

	/** Run for everyone when hook is shot regardless if it hits Soul or an obstacle */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookShotStart();

	/** Run on server when hook is fully retracted regardless if it hit Soul or an obstacle */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookShotEnd();

	/** Run for everyone when hook is fully retracted regardless if it hit Soul or an obstacle */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookShotEnd();
	
};
