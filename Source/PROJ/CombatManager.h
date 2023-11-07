// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpawnPoint.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"


class ASpawnPoint;

USTRUCT(BlueprintType, Category = "Enemy Spawn Wave")
struct FEnemyWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	TSubclassOf<ACharacter> EnemyClass;
	
	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	TArray<ASpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int NumEnemies;

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int AllowedRemainingEnemiesForNextWave;

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int TimeToNextWaveAfterEnemiesKilled;
};

UCLASS()
class PROJ_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACombatManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddEnemy(ACharacter* Enemy);

	void RemoveEnemy(ACharacter* Enemy);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemies")
	int NumActiveEnemies = 0;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<ASpawnPoint> SpawnPointBPClass;
	
	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<ASpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<FEnemyWave> Waves;

private:
	UPROPERTY(VisibleAnywhere, Category="Enemies")
	TArray<ACharacter*> Enemies;

	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
};
