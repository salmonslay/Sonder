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

	void SetUpInput(UEnhancedInputComponent* InputComp);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

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

private:

	UPROPERTY(EditAnywhere)
	class UInputAction* AttackInputAction;
	
	UPROPERTY(EditAnywhere)
	float Damage = 5.0f;

	UPROPERTY(EditAnywhere)
	float FireSpeed = 1.0f;
	
	/** How long time needs to pass between attacks */
	UPROPERTY(EditAnywhere)
	float ThrowCooldown = 0.5f;

	UPROPERTY(EditAnywhere)
	float ExplodeTime = 5.0f;

	UPROPERTY(Replicated)
	bool bCanThrow = true;

	UPROPERTY()
	bool bIsExploding = false;

	void EnableCanThrow();

	void DisableGrenade();

	void EnableGrenade();

	void StartCountdown();

	UPROPERTY()
	class ASoulCharacter* Player; 

	/** Locally run function called when player presses throw button */
	void Throw();
	
	/**
	 * Function executed on server ONLY when client attacks, called from client. This function will probably only call
	 * the multicast function unless we will have some functionality that should only be run on the server 
	 */ 
	UFUNCTION(Server, Reliable)
	void ServerRPCExplosion(); 

	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExplosion();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	

	
};
