// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "LightGrenade.generated.h"

UCLASS()
class PROJ_API ALightGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightGrenade();

	UFUNCTION()
	void ActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	UStaticMeshComponent* GrenadeMesh = nullptr;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USphereComponent* ExplosionArea = nullptr;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USphereComponent* CollisionArea = nullptr;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USphereComponent* PulseExplosionArea = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void ExplosionEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void ThrowEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void PulseExplosionEvent();

	UPROPERTY()
	class ASoulCharacter* Player;

	UPROPERTY()
	class APROJCharacter* PlayerBase;

	UPROPERTY()
	AController* Controller;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanThrow = true;

	void Throw(const float TimeHeld);

	/** How long time needs to pass between attacks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThrowCooldown = 5.0f;

	UPROPERTY()
	float MaxTimePressed;

	UPROPERTY(EditAnywhere)
	float DefaultDamage = 5.0f;
	
	UPROPERTY(EditAnywhere)
	float PulseDamageMultiplier = 2.0f;

	/**
 * Function executed on server ONLY when client attacks, called from client. This function will probably only call
 * the multicast function unless we will have some functionality that should only be run on the server 
 */ 
	UFUNCTION(Server, Reliable)
	void ServerRPCExplosion();

	void PulseExplosion();
	
	void IsChargingGrenade(const float TimeHeld);

	void DisableIndicator() const; 

private:

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	float Damage;
	
	/** How much a second of hold time should add to the throw velocity */
	UPROPERTY(EditAnywhere)
	float FireSpeedPerSecondHeld = 300000.0f;

	/** Min throw velocity, will start at this and then add up with FireSpeed */
	UPROPERTY(EditAnywhere)
	float StartThrowImpulse = 100000.f;

	/** Max throw velocity */
	UPROPERTY(EditAnywhere)
	float MaxThrowImpulse = 500000.f; 

	UPROPERTY(EditAnywhere)
	float ExplodeTimeSlow = 5.0f;

	UPROPERTY(EditAnywhere)
	float ExplodeTimeFast = 0.2f;

	bool bCanOverlap = false;

	UPROPERTY(Replicated)
	bool bIsExploding = false;

	UPROPERTY()
	bool bIsPulseExploding = false;
	
	/** The indicator class for the grenade */
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> IndicatorActorClass;

	UPROPERTY()
	AActor* Indicator;

	

	/** Needed an offset to get the indicator to match the grenades position */
	UPROPERTY(EditAnywhere)
	float IndicatorOffsetDivisor = 235.f;

	/** How long to be at max throw force before resetting to min */
	UPROPERTY(EditAnywhere)
	float TimeAtMaxThrowForce = 0.5f;

	/** How many times the player has looped with the max throw force */
	int MaxThrowIterations = 0;

	FTimerHandle ThrowIterTimerHandle; 

	// Charge moving up or down 
	bool bIncreasingCharge = true; 

	void IncreaseMaxThrowIterations(); 

	FVector GetLaunchForce(const float TimeHeld); 

	void EnableCanThrow();

	void DisableGrenade();

	void EnableGrenade();

	void GetPlayer();

	void StartCountdown(float TimeUntilExplosion);
	
	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExplosion();

	UFUNCTION(Server, Reliable)
	void ServerRPCThrow(const float TimeHeld); 

	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCThrow(const float TimeHeld);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
