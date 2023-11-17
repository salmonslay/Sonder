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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	UStaticMeshComponent* GrenadeMesh = nullptr;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly)
	USphereComponent* ExplosionArea = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void ExplosionEvent();

	UPROPERTY()
	class ASoulCharacter* Player;

	UPROPERTY()
	class APROJCharacter* PlayerBase;

	UPROPERTY()
	AController* Controller;

	UPROPERTY(Replicated)
	bool bCanThrow = true;

	void Throw();

	/** How long time needs to pass between attacks */
	UPROPERTY(EditAnywhere)
	float ThrowCooldown = 5.0f;


private:

	UPROPERTY()
	FTimerHandle TimerHandle; 
	
	UPROPERTY(EditAnywhere)
	float Damage = 5.0f;

	UPROPERTY(EditAnywhere)
	float FireSpeed = 8.0f;
	
	

	UPROPERTY(EditAnywhere)
	float ExplodeTimeSlow = 5.0f;

	UPROPERTY(EditAnywhere)
	float ExplodeTimeFast = 0.2f;

	

	bool bCanOverlap = false;

	UPROPERTY()
	bool bIsExploding = false;

	void EnableCanThrow();

	void DisableGrenade();

	void EnableGrenade();

	void StartCountdown(float TimeUntilExplosion);

	
	
	
	
	/**
	 * Function executed on server ONLY when client attacks, called from client. This function will probably only call
	 * the multicast function unless we will have some functionality that should only be run on the server 
	 */ 
	UFUNCTION(Server, Reliable)
	void ServerRPCExplosion(); 

	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExplosion();

	UFUNCTION(Server, Reliable)
	void ServerRPCThrow(); 

	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCThrow();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	

	
};
