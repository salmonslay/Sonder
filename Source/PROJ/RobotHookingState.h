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

	// UFUNCTION(Server, Reliable)
	// void ServerRPC_Enter(); 

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
	float HookShotTravelSpeed = 1000.f;

	float DefaultGravityScale = 1.75f;

	UPROPERTY()
	class UCharacterMovementComponent* MovementComponent;

	bool bShootingHookOutwards = false;
	
	UPROPERTY()
	class UCableComponent* HookCable;

	UPROPERTY(EditAnywhere)
	float ZTargetOffset = -20.f; 

	// Fail safe to cancel hook shot after set time 
	float FailSafeTimer = 0;

	/** Max time that the player can hook shot, will exit automatically after set time */
	UPROPERTY(EditAnywhere)
	float FailSafeLength = 2.f;

	/** The speed at which the hook cable should retract when it hits a blocking object */
	UPROPERTY(EditAnywhere)
	float RetractHookOnMissSpeed = 1000.f; 

	/** Switches state to the base state, ending the hook shot */
	void EndHookShot() const; 
	
	/** Returns false if there was a block */
	bool SetHookTarget();

	FHitResult DoLineTrace();

	/** Shoots out the physical hook, does not move player */
	void ShootHook();

	void StartTravelToTarget();

	/** Called on server when starting travel towards Soul (nothing blocking) */
	UFUNCTION(Server, Reliable)
	void ServerRPCStartTravel();

	/** Called on all players when starting travel towards Soul (nothing blocking) */
	UFUNCTION(Server, Reliable)
	void MulticastRPCStartTravel(); 

	/** Travels the player towards the set target locally */
	void TravelTowardsTarget(const float DeltaTime);

	/** Travels the player towards the set target on server. Unreliable since it's called each Tick */
	UFUNCTION(Server, Unreliable)
	void ServerTravelTowardsTarget(const float DeltaTime, const FVector Direction); 
	
	/** Called at the end of a hook shot IF the Robot collided with Soul */
	void CollidedWithSoul();

	/** Run on server when Robot collides with soul at the end of the hook shot */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookCollision();

	/** Run for everyone when Robot collides with soul at the end of the hook shot */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookCollision();

	/** Retracts the hook, handles both Soul hits and misses. Locally */
	void RetractHook(const float DeltaTime);

	// Unreliable because it is called each Tick 
	UFUNCTION(Server, Unreliable)
	void ServerRPC_RetractHook(const FVector& NewEndLocation);

	// Reliable because seem to be never called otherwise. Since it's called by an unreliable func it should be fine(?) 
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_RetractHook(const FVector& NewEndLocation); 

	/** Run on server when hook is shot regardless if it hits Soul or an obstacle */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookShotStart(UCableComponent* HookCableComp, const FVector& HookTarget, ARobotStateMachine* RobotChar);

	/** Run for everyone when hook is shot regardless if it hits Soul or an obstacle */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookShotStart(const FVector& HookTarget, ARobotStateMachine* RobotChar);

	/** Run on server when hook is fully retracted regardless if it hit Soul or an obstacle */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookShotEnd(UCableComponent* HookCableComp, ARobotStateMachine* RobotChar, const bool bResetVel);

	/** Run for everyone when hook is fully retracted regardless if it hit Soul or an obstacle */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookShotEnd(ARobotStateMachine* RobotChar);
	
};
