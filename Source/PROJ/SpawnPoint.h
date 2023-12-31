// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

class UCapsuleComponent;
class AEnemyCharacter;
class ACombatManager;

USTRUCT()
struct FEnemyToSpawn
{
	GENERATED_BODY()
	TSubclassOf<AEnemyCharacter> Class;	
};

UCLASS()
class PROJ_API ASpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnPoint();

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UPROPERTY(EditDefaultsOnly)
	float SpawnCheckFrequency = .5f;

	void AddEnemyToSpawn(TSubclassOf<AEnemyCharacter> EnemyClass);

	void TrySpawnNext();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSpawnEnemy();

	UFUNCTION(NetMulticast, Reliable)
	void DoSpawnEvent();

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY()
	ACombatManager* Manager = nullptr;

	bool bCombatOver = false; 
private:
	TQueue<FEnemyToSpawn> EnemiesToSpawn = TQueue<FEnemyToSpawn>();

	FTimerHandle SpawnCheckTimerHandle;

	FVector CalculateRetreatLocation();

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float RetreatLocationTraceDistance = 600.f;
};
