// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatDirector.generated.h"

class ACombatTriggeredBase;
class AEnemyCharacter;
class ACombatManager;

USTRUCT()
struct FSpawnStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	FName Name = FName();

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	float BaseCost = 0.f;

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	int BaseNumEnemies = 0;

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	float CostPerAdditionalEnemy = 0.f;

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	TArray<TSubclassOf<AEnemyCharacter>> EnemyClasses = TArray<TSubclassOf<AEnemyCharacter>>();

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	TArray<ACombatTriggeredBase*> WaveTriggeredActors = TArray<ACombatTriggeredBase*>();

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	bool bDoOnce = false;

	UPROPERTY(EditAnywhere, Category="SpawnParams")
	TArray<FName> UnlockedSpawnTypes = TArray<FName>();

	UPROPERTY(EditAnywhere, Category="Weight")
	bool bActiveEnemiesAddWeight = false;

	UPROPERTY(VisibleAnywhere, Category="Weight")
	int WavesUnpicked = 0;
	
	bool operator==(const FSpawnStruct&) const;
};

UCLASS()
class PROJ_API ACombatDirector : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACombatDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	ACombatManager* Manager;

	UPROPERTY(EditAnywhere, Category="Budget")
	float CurrentBudget = 0.f;

	UPROPERTY(EditAnywhere, Category="Budget")
	float BaseBudgetPerSecond = 1.f;

	UPROPERTY(EditAnywhere, Category="Budget")
	float BudgetMultiplier = 1.f;

	UPROPERTY(EditAnywhere, Category="Budget")
	float BudgetGrowthMultiplier = 1.1f;

	UPROPERTY(EditAnywhere, Category="Budget")
	float BudgetGrowthTimer = 10.f;
	
	UPROPERTY(EditAnywhere, Category="Spawn")
	float MinSpawnWait = 4.f;

	UPROPERTY(EditAnywhere, Category="Spawn")
	float MaxSpawnWait = 8.f;

	UPROPERTY(EditAnywhere, Category="Spawn")
	float UnderSpendSpawnWait = 0.5f;

	UPROPERTY(EditAnywhere, Category="Spawn")
	float SpawnCheckFrequencyIncreaseTimer = 20.f;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<FSpawnStruct> SpawnTypes;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TMap<FName, FSpawnStruct> UnlockableSpawnTypes;

	UPROPERTY(EditAnywhere, Category="Weight")
	float CostWeightMultiplier = 1.f;

	UPROPERTY(EditAnywhere, Category="Weight")
	float ActiveEnemiesWeightMultiplier = 1.f;

	UPROPERTY(EditAnywhere, Category="Weight")
	float UnpickedWeightMultiplier = 2.f;

	void SpendBudget();

	void IncreaseBudgetMultiplier();
	
private:
	
	int CalculateSpawnWeight(const FSpawnStruct& Spawn) const;

	int WeightedRandomSpawnTypeIndex(int TotalWeight, int MaxValidIndex);

	void MakeWaves(const FSpawnStruct& Spawn);
	
	FTimerHandle SpendBudgetTimerHandle;
	
	FTimerHandle BudgetGrowthTimerHandle;

	FTimerHandle SpawnFrequencyIncreaseTimerHandle;

	bool bInitialized = false;

	TArray<int> SpawnPointIndices = TArray<int>();

};
