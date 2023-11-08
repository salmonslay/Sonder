// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BoxComponent.h"
#include "CombatManager.h"


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
		WavesQueue.Enqueue(Wave);
	}
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
/*
void ACombatManager::GatherOverlappingSpawnPoints()
{
	TArray<AActor*> OverlappingSpawnPoints;
	ManagerBounds->GetOverlappingActors(OverlappingSpawnPoints, ASpawnPoint::StaticClass());
	for (AActor* SP : OverlappingSpawnPoints)
	{
		ASpawnPoint* SpawnPoint = Cast<ASpawnPoint>(SP);
		if(SpawnPoint)
		{
			SpawnPoints.Add(SpawnPoint);
		}
	}
}
*/
void ACombatManager::HandleSpawn()
{
	
}

void ACombatManager::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	//Needs destructors for the spawn points when they are removed from the array.
	/*
	if(PropertyChangedEvent.GetPropertyName() == TEXT("SpawnPoints"))
	{
		const int Index = PropertyChangedEvent.GetArrayIndex(TEXT("SpawnPoints"));
		switch (PropertyChangedEvent.ChangeType)
		{
		case EPropertyChangeType::ArrayAdd:
			if(SpawnPoints.Num() > Index && SpawnPoints[Index] == nullptr)
			{
				SpawnPoints[Index] = GetWorld()->SpawnActor<ASpawnPoint>(SpawnPointBPClass, GetActorLocation(), GetActorRotation());
				if(SpawnPoints[Index])
				{
					SpawnPoints[Index]->SetActorTransform(GetActorTransform());
					SpawnPoints[Index]->Rename(TEXT("SpawnPoint" + Index));
				}
			}
			return;
		case EPropertyChangeType::ArrayRemove:
			PropertyChangedEvent.Property->BeginDestroy();
			return;
		default:
			return;
		}
	}
	*/
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

