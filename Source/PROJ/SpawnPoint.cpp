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
		GetWorldTimerManager().SetTimer(SpawnCheckTimerHandle, this, &ASpawnPoint::TrySpawnNext, SpawnCheckFrequency, true);
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
				SpawnedEnemy->InitializeControllerFromManager();
				SpawnedEnemy->Manager = Manager;
			}
			Manager->AddEnemy(SpawnedEnemy);
		}
	}
	if(bCombatOver)
	{
		GetWorldTimerManager().ClearTimer(SpawnCheckTimerHandle);
	}
}

