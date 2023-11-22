// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "DamageZone.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJ_API ADamageZone : public ATriggerBox
{
	GENERATED_BODY()

	ADamageZone();

public:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// The time until the player takes the first damage
	UPROPERTY(EditAnywhere)
	float TimeUntilFirstDamage = 0.5f;

	// The time between each damage
	UPROPERTY(EditAnywhere)
	float TimeBetweenDamage = 1.5f;

	// The amount of damage the player(s) takes each time
	UPROPERTY(EditAnywhere)
	float DamageAmount = 1;

	// Whether or not the player dies instantly when entering the zone
	UPROPERTY(EditAnywhere)
	bool bInstantKill = false;

	// Whether or not the player can only take damage once per overlap
	UPROPERTY(EditAnywhere)
	bool bOnlyDamageOnce = false;

	// The global game time at which this zone last damaged a player
	float LastDamageTime = -10000.f; // We start in the negative so that the player takes damage immediately

	/**
	 * The players that are currently in the zone. 
	 */
	UPROPERTY()
	TArray<AActor*> PlayerActors;

	UPROPERTY()
	TArray<AActor*> PlayersDamagedThisOverlap;
};
