// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"

#include "CombatTrigger.h"
#include "EnemyCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ACombatManager::ACombatManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ManagerBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	RootComponent = ManagerBounds;
}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();

	//GatherOverlappingSpawnPoints();
	for(FEnemyWave Wave : Waves)
	{
		WavesQueue.Add(Wave);
	}
	for(ASpawnPoint* SpawnPoint : SpawnPoints)
	{
		SpawnPoint->Manager = this;
	}
	for(ACombatTrigger* CombatTrigger : CombatTriggers)
	{
		CombatTrigger->Manager = this;
	}
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bCombatStarted && !bCombatEnded && GetLocalRole() == ROLE_Authority && !WavesQueue.IsEmpty() &&
		!bWaitingForWave && NumActiveEnemies <= WavesQueue[0].AllowedRemainingEnemiesForWave)
	{
		if (float Wait = WavesQueue[0].TimeToWaveAfterEnemiesKilled <= 0)
		{
			HandleSpawn();
		}
		else
		{
			bWaitingForWave = true;
			GetWorldTimerManager().SetTimer(WaveWaitTimerHandle, this, &ACombatManager::HandleSpawn, Wait, false);
		}
	}
}

void ACombatManager::AddEnemy(AEnemyCharacter* Enemy)
{
	Enemies.Emplace(Enemy);
}

void ACombatManager::RemoveEnemy(AEnemyCharacter* Enemy)
{
	Enemies.Remove(Enemy);
	NumActiveEnemies--;
	if(WavesQueue.IsEmpty() && NumActiveEnemies <= 0 && GetLocalRole() == ROLE_Authority)
	{
		for(ASpawnPoint* SpawnPoint : SpawnPoints)
		{
			SpawnPoint->bCombatOver = true;
		}
		bCombatEnded = true;
		GetWorldTimerManager().ClearTimer(WaveWaitTimerHandle);
		OnCombatEnd();
	}
}

void ACombatManager::StartCombat()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bCombatStarted = true;
		OnCombatBegin();
	}
}

void ACombatManager::OnRep_CombatStarted()
{
	OnCombatBegin();
}

void ACombatManager::OnRep_CombatEnded()
{
	OnCombatEnd();
}

void ACombatManager::HandleSpawn()
{
	bWaitingForWave = false;
	const FEnemyWave Wave = WavesQueue.Pop();
	NumActiveEnemies += Wave.NumEnemies;
	for(int i = 0; i < Wave.NumEnemies; i++)
	{
		SpawnPoints[i % SpawnPoints.Num()]->AddEnemyToSpawn(Wave.EnemyClass);
	}
}

void ACombatManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
