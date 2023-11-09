// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"
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
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bCombatStarted && !bCombatEnded && !WavesQueue.IsEmpty() && !bWaitingForWave &&
		NumActiveEnemies <= WavesQueue[0].AllowedRemainingEnemiesForWave)
	{
		if (WavesQueue[0].TimeToWaveAfterEnemiesKilled <= 0)
		{
			//Handle wave immediately
		}
		else
		{
			bWaitingForWave = true;
			//Handle wave on a timer
		}
	}
}

void ACombatManager::AddEnemy(ACharacter* Enemy)
{
	Enemies.Emplace(Enemy);
}

void ACombatManager::RemoveEnemy(ACharacter* Enemy)
{
	Enemies.Remove(Enemy);
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
	FEnemyWave Wave = WavesQueue.Pop();
	for(int i = 0; i < Wave.NumEnemies; i++)
	{
		
	}
}

void ACombatManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
