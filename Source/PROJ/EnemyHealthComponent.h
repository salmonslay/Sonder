// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHealthComponent.h"
#include "EnemyHealthComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJ_API UEnemyHealthComponent : public UBaseHealthComponent
{
	GENERATED_BODY()

public:

	UEnemyHealthComponent();

	//virtual float TakeDamage(float DamageAmount) override;

	virtual void IDied() override;

	virtual float TakeDamage(float DamageAmount) override;

protected:

	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter;

	/** Function run on server when a player takes damage */
	UFUNCTION(Server, Reliable)
	void ServerRPCDamageTaken(float DamageTaken);

	/** Function run on each game instance when damage is taken, run on server and client */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDamageTaken(float DamageTaken);

	/** Function run on server when a player dies */
	UFUNCTION(Server, Reliable) 
	void ServerRPCPlayerDied();

	/** Function run on each game instance when player dies, run on server and client */
	UFUNCTION(NetMulticast, Reliable) 
	void MulticastRPCPlayerDied(); 
};
