// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"


// Sets default values
ACombatManager::ACombatManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	
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

void ACombatManager::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	//Needs destructors for the spawn points when they are removed from the array.
	/*
	if(PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd &&
		PropertyChangedEvent.GetPropertyName() == TEXT("SpawnPoints"))
	{
		const int Index = PropertyChangedEvent.GetArrayIndex(TEXT("SpawnPoints"));
		if(SpawnPoints.Num() > Index && SpawnPoints[Index] == nullptr)
		{
			SpawnPoints[Index] = GetWorld()->SpawnActor<ASpawnPoint>(SpawnPointBPClass, GetActorLocation(), GetActorRotation());
			if(SpawnPoints[Index])
			{
				SpawnPoints[Index]->SetActorTransform(GetActorTransform());
			}
		}
	}
	*/
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

