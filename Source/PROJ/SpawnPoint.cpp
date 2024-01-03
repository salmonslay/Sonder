// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

#include "CombatManager.h"
#include "EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ASpawnPoint::ASpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
}

void ASpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ASpawnPoint::AddEnemyToSpawn(const TSubclassOf<AEnemyCharacter> EnemyClass)
{
	FEnemyToSpawn Enemy;
	Enemy.Class = EnemyClass;
	EnemiesToSpawn.Enqueue(Enemy);
	if(!GetWorldTimerManager().TimerExists(SpawnCheckTimerHandle))
	{
		TrySpawnNext();
		GetWorldTimerManager().SetTimer(SpawnCheckTimerHandle, this, &ASpawnPoint::TrySpawnNext, SpawnCheckFrequency, true);
	}
}

void ASpawnPoint::TrySpawnNext()
{
	if(!EnemiesToSpawn.IsEmpty() && !bCombatOver)
	{
		AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemiesToSpawn.Peek()->Class, GetActorLocation(), GetActorRotation());
		if(SpawnedEnemy)
		{
			EnemiesToSpawn.Pop();
			if (!SpawnedEnemy->bIsControllerInitialized)
			{
				SpawnedEnemy->Manager = Manager;
				//SpawnedEnemy->RetreatLocation = GetActorLocation();
				SpawnedEnemy->RetreatLocation = CalculateRetreatLocation();
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

FVector ASpawnPoint::CalculateRetreatLocation()
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = FVector(StartLocation.X, StartLocation.Y , StartLocation.Z- RetreatLocationTraceDistance);

	TArray<TEnumAsByte<EObjectTypeQuery>> LineTraceObjects;
	LineTraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		
	const bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, StartLocation, EndLocation, LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FColor::Red, FColor::Blue, 10.f);

	if (bHit)
	{
		return HitResult.GetActor()->GetActorLocation();
	}
	return GetActorLocation();
}

void ASpawnPoint::DoSpawnEvent_Implementation()
{
	OnSpawnEnemy();
}
