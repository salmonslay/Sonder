// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharState.h"
#include "RobotHookingState.generated.h"

/**
 * State when the Robot requests to Hook 
 */
UCLASS()
class SONDER_API URobotHookingState : public UPlayerCharState
{
	GENERATED_BODY()

public:

	URobotHookingState(); 

	virtual void Enter() override;

	// Sets variables that are meant to be set once. Also ensures they are set for client and server as opposed to Enter 
	virtual void BeginPlay() override;

	virtual void Update(const float DeltaTime) override;

	virtual void Exit() override;

	/** Switches state to the base state, ending the hook shot */
	void EndHookShot(const bool bEndFromDash = false) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns true if the player is currently hook shotting, with or without a valid target */
	UFUNCTION(BlueprintPure)
	bool IsHookShotting() const { return bHookShotActive; } 

	/** Returns the location at which to move the hook arm during hook shots */
	UFUNCTION(BlueprintPure)
	FVector GetCurrentHookArmLocation() const { return HookArmLocation; }

	/** Currently travelling towards a valid target */
	UFUNCTION(BlueprintPure)
	bool IsTravellingToTarget() const { return bTravellingTowardsTarget; }

	bool HasValidTarget() const { return IsValid(CurrentTargetActor); }

	bool IsTargetSoul() const { return bHookTargetIsSoul; }

private:
	
	/** The location for the arm, used to animate the Robot */
	UPROPERTY(Replicated)
	FVector HookArmLocation;  

	UPROPERTY()
	AActor* SoulCharacter; 
	
	UPROPERTY(Replicated)
	FVector CurrentHookTargetLocation;

	UPROPERTY()
	class ARobotStateMachine* RobotCharacter;

	UPROPERTY(Replicated)
	bool bTravellingTowardsTarget = false;

	bool bHookTargetIsSoul = false;

	/** What actor is currently targeted (Soul or a hook point), or nullptr if none */
	UPROPERTY()
	AActor* CurrentTargetActor; 

	/** How close the player needs to be for it to be considered as reached target */
	float ReachedTargetDistTolerance = 50.f;
	
	/** How fast to travel */
	UPROPERTY(EditAnywhere)
	float HookShotTravelSpeed = 1250.f;

	float DefaultGravityScale = 1.75f;

	UPROPERTY()
	class UCharacterMovementComponent* MovementComponent;

	/** In process of shooting the hook towards the target (blocked or valid) */
	bool bShootingHookOutwards = false;

	UPROPERTY(EditAnywhere)
	float ZSoulTargetOffset = 25.f; 

	// Fail safe to cancel hook shot after set time 
	float FailSafeTimer = 0;

	/** Max time that the player can hook shot, will exit automatically after set time */
	UPROPERTY(EditAnywhere)
	float FailSafeLength = 2.f;

	/** The speed at which the hook cable should retract when it hits a blocking object */
	UPROPERTY(EditAnywhere)
	float RetractHookOnMissSpeed = 2000.f;

	/** How far the hook should shoot out (max distance) when it hits a block or has no valid target */
	UPROPERTY(EditAnywhere)
	float MaxHookShotDistanceOnBlock = 1000.f;

	/** Spawns this actor on collision with Soul. The spawned class handles damage */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHookExplosionActor> ExplosionClassToSpawnOnCollWithSoul;

	/** Location at the start of the hook shot, used to determine travel length */
	FVector StartLocation;

	/** Velocity divisor to apply when reaching a static hook */
	UPROPERTY(EditAnywhere)
	float VelocityDivOnReachedHook = 1.2f; 

	UPROPERTY(EditAnywhere)
	float HookTravelDamageAmount = 3.f;

	/** The speed at which to move the hook cable when moving outwards */
	UPROPERTY(EditAnywhere)
	float OutwardsHookShotSpeed = 4000.f;

	/** Currently shooting a hook - true/false */
	UPROPERTY(Replicated)
	bool bHookShotActive = false;
	
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* GrabEffect;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* HookShotTravelEffect;

	/** The component that spawns when spawning the travel effect, used to disable the effect when travel is done */
	UPROPERTY()
	class UNiagaraComponent* HookShotTravelComponent; 
	
	/** Returns the HookTarget if there is no available target, ensuring hook is shot forwards */
	FVector GetTargetOnNothingInFront() const;
	
	/** Returns false if there was a block */
	bool SetHookTarget();

	/** Simply sets the target location on the server so it replicates to clients */
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetHookTarget(const FVector& NewTarget); 

	/** Shoots out the physical hook, does not move player. Run each Tick */
	void ShootHook(const float DeltaTime);

	/** Shoots out the physical hook on Server, does not move player. Called each Tick */
	UFUNCTION(Server, Unreliable)
	void ServerRPC_ShootHook(const FVector& HookTarget);

	void StartTravelToTarget();

	/** Called on server when starting travel towards Soul (nothing blocking) */
	UFUNCTION(Server, Reliable)
	void ServerRPCStartTravel();

	/** Called on all players when starting travel towards Soul (nothing blocking) */
	UFUNCTION(NetMulticast, Reliable)
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

	/** Run once when starting to shoot the hook outwards towards the target */
	void StartShootHook(); 
	
	/** Run on server when hook is shot regardless if it hits Soul or an obstacle */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookShotStart(const bool bValidHookTarget);

	/** Run for everyone when hook is shot regardless if it hits Soul or an obstacle */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookShotStart(const bool bValidHookTarget);

	/** Run on server when hook is fully retracted regardless if it hit Soul or an obstacle */
	UFUNCTION(Server, Reliable)
	void ServerRPCHookShotEnd(const FVector& NewVel);

	/** Run for everyone when hook is fully retracted regardless if it hit Soul or an obstacle */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHookShotEnd();

	UFUNCTION()
	void ActorOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); 

	UFUNCTION(Server, Reliable)
	void ServerRPC_DamageActor(AActor* ActorToDamage); 
	
};
