// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatDirector.h"

#include "CombatManager.h"


bool FSpawnStruct::operator==(const FSpawnStruct& Other) const
{
	return Name == Other.Name;
}

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

	SpawnTypes.Sort([](const FSpawnStruct SS1, const FSpawnStruct SS2){return SS1.BaseCost < SS2.BaseCost;});
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

	if(Manager->bEndlessMode && Manager->IsCombatStarted() && !Manager->IsCombatEnded() && GetLocalRole() == ROLE_Authority)
	{
		if(!bInitialized)
		{
			bInitialized = true;
			SpendBudget();
			//GetWorldTimerManager().SetTimer(SpendBudgetTimerHandle, this, &ACombatDirector::SpendBudget, FMath::RandRange(MinSpawnWait, MaxSpawnWait), false);
			GetWorldTimerManager().SetTimer(SpawnFrequencyIncreaseTimerHandle, Manager, &ACombatManager::IncreaseSpawnCheckFrequency, SpawnCheckFrequencyIncreaseTimer, true);
			GetWorldTimerManager().SetTimer(BudgetGrowthTimerHandle, this, &ACombatDirector::IncreaseBudgetMultiplier, BudgetGrowthTimer, true);
		}
		CurrentBudget += BaseBudgetPerSecond * BudgetMultiplier * DeltaTime;
	}
}

void ACombatDirector::SpendBudget()
{
	int MaxValidIndex = -1;
	int TotalWeight = 0;
	for (FSpawnStruct Spawn : SpawnTypes)
	{
		if(CurrentBudget >= Spawn.BaseCost)
		{
			MaxValidIndex++;
			TotalWeight += CalculateSpawnWeight(Spawn);
		}
		Spawn.WavesUnpicked++;
	}
	float WaitTimeForNextCheck = FMath::RandRange(MinSpawnWait, MaxSpawnWait);
	if(MaxValidIndex >= 0)
	{
		const int Index = WeightedRandomSpawnTypeIndex(TotalWeight, MaxValidIndex);
		FSpawnStruct Spawn = SpawnTypes[Index];
		Spawn.WavesUnpicked = 0;
		//UE_LOG(LogTemp, Warning, TEXT("Budget before spending: %f"), CurrentBudget);
		CurrentBudget -= Spawn.BaseCost;
		int ExtraEnemies = 0;
		if(Spawn.CostPerAdditionalEnemy > 0)
		{
			ExtraEnemies = CurrentBudget / Spawn.CostPerAdditionalEnemy;
			//UE_LOG(LogTemp, Warning, TEXT("Number of extra enemies to spawn: %i"), ExtraEnemies);
			CurrentBudget -= ExtraEnemies * Spawn.CostPerAdditionalEnemy;
		}
		if(Spawn.EnemyClasses.Num() <= 0)
		{
			FEnemyWave Wave = FEnemyWave();
			Wave.AllowedRemainingEnemiesForWave = -1;
			Wave.WaveStartedTriggeredActors = Spawn.WaveTriggeredActors;
			Manager->AddWave(Wave);
		}
		for(const TSubclassOf<AEnemyCharacter> EnemyClass : Spawn.EnemyClasses)
		{
			FEnemyWave Wave = FEnemyWave();
			Wave.EnemyClass = EnemyClass;
			Wave.NumEnemies = Spawn.BaseNumEnemies + ExtraEnemies;
			Wave.AllowedRemainingEnemiesForWave = -1;
			Wave.WaveStartedTriggeredActors = Spawn.WaveTriggeredActors;

			//todo: there has to be a better way to do this, would also like it if there was no overlap when spawning two enemy types
			const int StartSpawnPointIndex = FMath::RandRange(0, Manager->SpawnPoints.Num() - 2);
			const int EndSpawnPointIndex = FMath::RandRange(StartSpawnPointIndex + 1,
				FMath::Min(Manager->SpawnPoints.Num(), StartSpawnPointIndex + Wave.NumEnemies));
			//UE_LOG(LogTemp, Warning, TEXT("Starting and ending spawn point indexes: %i and %i"), StartSpawnPointIndex, EndSpawnPointIndex);
			for(int i = StartSpawnPointIndex; i < EndSpawnPointIndex; i++)
			{
				Wave.SpawnPoints.Emplace(Manager->SpawnPoints[i]);
			}
			Manager->AddWave(Wave);
		}
		if(CurrentBudget > Spawn.BaseCost)
		{
			WaitTimeForNextCheck = UnderSpendSpawnWait;
		}
		if(Spawn.bDoOnce)
		{
			for(FName AddedSpawn : Spawn.UnlockedSpawnTypes)
			{
				SpawnTypes.Add(UnlockableSpawnTypes[AddedSpawn]);
			}
			SpawnTypes.Remove(Spawn);
			SpawnTypes.Sort([](const FSpawnStruct SS1, const FSpawnStruct SS2){return SS1.BaseCost < SS2.BaseCost;});
		}
		//UE_LOG(LogTemp, Warning, TEXT("Budget after spending: %f"), CurrentBudget);
	}
	GetWorldTimerManager().SetTimer(SpendBudgetTimerHandle, this, &ACombatDirector::SpendBudget, WaitTimeForNextCheck);
}

void ACombatDirector::IncreaseBudgetMultiplier()
{
	BudgetMultiplier *= BudgetGrowthMultiplier;
	//UE_LOG(LogTemp, Warning, TEXT("Increased budget multiplier, current multiplier: %f"), BudgetMultiplier);
}

int ACombatDirector::CalculateSpawnWeight(const FSpawnStruct& Spawn) const
{
	const int Weight = Spawn.BaseCost * CostWeightMultiplier + Spawn.WavesUnpicked * UnpickedWeightMultiplier +
		Spawn.bActiveEnemiesAddWeight * Manager->NumActiveEnemies * ActiveEnemiesWeightMultiplier;
	UE_LOG(LogTemp, Error, TEXT("%s has weight: %i"), *Spawn.Name.ToString(), Weight);
	return Weight;
}

int ACombatDirector::WeightedRandomSpawnTypeIndex(int TotalWeight, int MaxValidIndex)
{
	int RandVal = rand() % TotalWeight;
	for(int i = 0; i <= MaxValidIndex; i++)
	{
		const int SpawnWeight = CalculateSpawnWeight(SpawnTypes[i]);
		if(RandVal <= SpawnWeight)
		{
			return i;
		}
		RandVal -= SpawnWeight;
	}
	return -1;
}

void ACombatDirector::MakeWaves(const FSpawnStruct& Spawn)
{
	if(SpawnPointIndices.IsEmpty())
	{
		for(int i = 0; i < Manager->SpawnPoints.Num(); i++)
		{
			SpawnPointIndices.Emplace(i);
		}
	}
}
