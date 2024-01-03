// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatDirector.h"

#include "CombatManager.h"
#include "NewPlayerHealthComponent.h"
#include "PROJCharacter.h"
#include "SonderGameState.h"


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
		CurrentBudget -= Spawn.BaseCost;
		int ExtraEnemies = 0;
		if(Spawn.CostPerAdditionalEnemy > 0)
		{
			ExtraEnemies = CurrentBudget / Spawn.CostPerAdditionalEnemy;
			CurrentBudget -= ExtraEnemies * Spawn.CostPerAdditionalEnemy;
		}
		if(!Spawn.WaveTriggeredActors.IsEmpty())
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

			int MaxNumSpawnPoints = FMath::Min(Manager->SpawnPoints.Num() / Spawn.EnemyClasses.Num(), Wave.NumEnemies);
			int NumSpawnPoints = FMath::RandRange(1, MaxNumSpawnPoints);
			AddSpawnPointsToWave(Wave, NumSpawnPoints);

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
	}
	GetWorldTimerManager().SetTimer(SpendBudgetTimerHandle, this, &ACombatDirector::SpendBudget, WaitTimeForNextCheck);
}

void ACombatDirector::IncreaseBudgetMultiplier()
{
	BudgetMultiplier *= BudgetGrowthMultiplier;
}

int ACombatDirector::CalculateSpawnWeight(const FSpawnStruct& Spawn) const
{
	int Weight = Spawn.BaseCost * CostWeightMultiplier + Spawn.WavesUnpicked * UnpickedWeightMultiplier +
		Spawn.bActiveEnemiesAddWeight * Manager->NumActiveEnemies * ActiveEnemiesWeightMultiplier;
	if(Spawn.bPlayerHealthReducesWeight)
	{
		int HealthWeight = 50 - FMath::Min(
			Cast<ASonderGameState>(GetWorld()->GetGameState())->GetServerPlayer()->NewPlayerHealthComponent->GetHealth(),
			Cast<ASonderGameState>(GetWorld()->GetGameState())->GetClientPlayer()->NewPlayerHealthComponent->GetHealth());
		Weight += HealthWeight * PlayerHealthWeightMultiplier;
	}
	return FMath::Max(Weight, 0);
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

void ACombatDirector::AddSpawnPointsToWave(FEnemyWave& Wave, int NumSpawnPoints)
{
	for(int i = 0; i < NumSpawnPoints; i++)
	{
		if(SpawnPointIndices.IsEmpty())
		{
			for(int j = 0; j < Manager->SpawnPoints.Num(); j++)
			{
				SpawnPointIndices.Emplace(j);
			}
		}
		int RandomIndex = SpawnPointIndices[rand() % SpawnPointIndices.Num()];
		Wave.SpawnPoints.Emplace(Manager->SpawnPoints[RandomIndex]);
		SpawnPointIndices.Remove(RandomIndex);
	}
}
