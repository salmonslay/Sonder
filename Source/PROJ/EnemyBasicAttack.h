// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "EnemyBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UEnemyBasicAttack : public UBoxComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UEnemyBasicAttack();

	/** Locally run function called when player presses attack button */
	void Attack();

	bool CanAttack() const { return bCanAttack; }  

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/** Damage inflicted on enemies */
	UPROPERTY(EditAnywhere)
	float Damage = 5.f;

	/** How long time needs to pass between attacks */
	UPROPERTY(EditAnywhere)
	float AttackCooldown = 0.5f; 

	UPROPERTY(Replicated)
	bool bCanAttack = true;

	void EnableCanAttack();

	UPROPERTY()
	class AShadowCharacter* Owner; 
	
	/**
	 * Function executed on server ONLY when client attacks, called from client. This function will probably only call
	 * the multicast function unless we will have some functionality that should only be run on the server 
	 */ 
	UFUNCTION(Server, Reliable)
	void ServerRPCAttack(); 

	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
