// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"

#include "CombatTrigger.h"
#include "CombatTriggeredBase.h"
#include "EnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACombatManager::ACombatManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ManagerBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	RootComponent = ManagerBounds;

	bReplicates = true;
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
		float Wait = WavesQueue[0].TimeToWaveAfterEnemiesKilled;
		if (Wait <= 0)
		{
			HandleSpawn();
			UE_LOG(LogTemp, Warning, TEXT("Delay wpanf %f"), Wait);
		}
		else
		{
			bWaitingForWave = true;
			GetWorldTimerManager().SetTimer(WaveWaitTimerHandle, this, &ACombatManager::HandleSpawn, Wait, false, Wait);
		}
	}
}

void ACombatManager::AddEnemy(AEnemyCharacter* Enemy)
{
	Enemies.Emplace(Enemy);
	Enemy->SetGridPointer(Grid);
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
		if(EndCombatTriggeredActor)
			EndCombatTriggeredActor->TriggeredEvent();
	}
}

void ACombatManager::StartCombat()
{
	if (GetLocalRole() == ROLE_Authority && !bCombatStarted)
	{
		bCombatStarted = true;
		OnCombatBegin();
		if(StartCombatTriggeredActor)
			StartCombatTriggeredActor->TriggeredEvent();
	}
}

void ACombatManager::OnRep_CombatStarted()
{
	OnCombatBegin();
	if(StartCombatTriggeredActor)
		StartCombatTriggeredActor->TriggeredEvent();
}

void ACombatManager::OnRep_CombatEnded()
{
	OnCombatEnd();
	if(EndCombatTriggeredActor)
		EndCombatTriggeredActor->TriggeredEvent();
}

void ACombatManager::HandleSpawn()
{
	bWaitingForWave = false;
	const FEnemyWave Wave = WavesQueue[0];
	if(Wave.SpawnPoints.Num() > 0)
	{
		NumActiveEnemies += Wave.NumEnemies;
		for(int i = 0; i < Wave.NumEnemies; i++)
		{
			Wave.SpawnPoints[i % Wave.SpawnPoints.Num()]->AddEnemyToSpawn(Wave.EnemyClass);
		}
	}
	if(Wave.WaveStartedTriggeredActor)
		Wave.WaveStartedTriggeredActor->TriggeredEvent();
	WavesQueue.RemoveAt(0);
	CurrentWave++;
}

void ACombatManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACombatManager, bCombatStarted);
	DOREPLIFETIME(ACombatManager, bCombatEnded);
}
