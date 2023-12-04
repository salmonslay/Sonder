// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatDirector.h"

#include "CombatManager.h"


// Sets default values
ACombatDirector::ACombatDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACombatDirector::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACombatDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

// Called every frame
void ACombatDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Manager->bEndlessMode && Manager->IsCombatStarted())
	{
		if(!bInitialized)
		{
			bInitialized = true;
			GetWorldTimerManager().SetTimer(SpendBudgetTimerHandle, this, &ACombatDirector::SpendBudget, FMath::RandRange(MinSpawnWait, MaxSpawnWait), false);
			GetWorldTimerManager().SetTimer(SpawnFrequencyIncreaseTimerHandle, Manager, &ACombatManager::IncreaseSpawnCheckFrequency, SpawnCheckFrequencyIncreaseTimer, true);
			GetWorldTimerManager().SetTimer(BudgetGrowthTimerHandle, this, &ACombatDirector::IncreaseBudgetMultiplier, BudgetGrowthTimer, true);
		}
		CurrentBudget += BaseBudgetPerSecond * BudgetMultiplier * DeltaTime;
	}
}

void ACombatDirector::SpendBudget()
{
	//todo: limit to only choosing affordable spawn types
	const int Index = FMath::RandRange(0, SpawnTypes.Num() - 1);
	FSpawnStruct Spawn = SpawnTypes[Index];
	if(CurrentBudget > Spawn.BaseCost)
	{
		CurrentBudget -= Spawn.BaseCost;
		const int ExtraEnemies = CurrentBudget / Spawn.CostPerAdditionalEnemy;
		CurrentBudget -= ExtraEnemies * Spawn.CostPerAdditionalEnemy;
		for(const TSubclassOf<AEnemyCharacter> EnemyClass : Spawn.EnemyClasses)
		{
			FEnemyWave Wave = FEnemyWave();
			Wave.EnemyClass = EnemyClass;
			Wave.NumEnemies = Spawn.BaseNumEnemies + ExtraEnemies;
			Wave.AllowedRemainingEnemiesForWave = -1;
			Wave.WaveStartedTriggeredActors = Spawn.WaveTriggeredActors;

			//todo: there has to be a better way to do this, would also like it if there was no overlap when spawning two enemy types
			const int StartSpawnPointIndex = FMath::RandRange(0, Manager->SpawnPoints.Num() - 2);
			const int EndSpawnPointIndex = FMath::RandRange(StartSpawnPointIndex,
				FMath::Min(Manager->SpawnPoints.Num(), StartSpawnPointIndex + Wave.NumEnemies));
			for(int i = StartSpawnPointIndex; i < EndSpawnPointIndex; i++)
			{
				Wave.SpawnPoints.Emplace(Manager->SpawnPoints[i]);
			}
			Manager->AddWave(Wave);
		}
	}
	GetWorldTimerManager().SetTimer(SpendBudgetTimerHandle, this, &ACombatDirector::SpendBudget, FMath::RandRange(MinSpawnWait, MaxSpawnWait));
}

void ACombatDirector::IncreaseBudgetMultiplier()
{
	BudgetMultiplier *= BudgetGrowthMultiplier;
}

