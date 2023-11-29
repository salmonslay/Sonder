// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

#include "CombatManager.h"
#include "EnemyCharacter.h"
#include "Components/CapsuleComponent.h"


// Sets default values
ASpawnPoint::ASpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

// Called every frame
void ASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawnPoint::AddEnemyToSpawn(TSubclassOf<AEnemyCharacter> EnemyClass)
{
	FEnemyToSpawn Enemy;
	Enemy.Class = EnemyClass;
	EnemiesToSpawn.Enqueue(Enemy);
	if(!GetWorldTimerManager().TimerExists(SpawnCheckTimerHandle))
	{
		TrySpawnNext();
		if(!EnemiesToSpawn.IsEmpty())
		{
			GetWorldTimerManager().SetTimer(SpawnCheckTimerHandle, this, &ASpawnPoint::TrySpawnNext, SpawnCheckFrequency, true);
		}
	}
}

void ASpawnPoint::TrySpawnNext()
{
	if(!EnemiesToSpawn.IsEmpty())
	{
		AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemiesToSpawn.Peek()->Class, GetActorLocation(), GetActorRotation());
		if(SpawnedEnemy)
		{
			EnemiesToSpawn.Pop();
			if (!SpawnedEnemy->bIsControllerInitialized)
			{
				SpawnedEnemy->Manager = Manager;
				SpawnedEnemy->SpawnPosition = GetActorLocation();
				SpawnedEnemy->InitializeControllerFromManager();
			}
			Manager->AddEnemy(SpawnedEnemy);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnCheckTimerHandle);
	}
}

void ASpawnPoint::DoSpawnEvent_Implementation()
{
	OnSpawnEnemy();
}

