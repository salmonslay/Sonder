// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpawnPoint.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"


class ACombatTrigger;
class AEnemyCharacter;
class UBoxComponent;
class ASpawnPoint;

USTRUCT(BlueprintType, Category = "Enemy Spawn Wave")
struct FEnemyWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	TSubclassOf<AEnemyCharacter> EnemyClass;
	
	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	TArray<ASpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int NumEnemies;

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int AllowedRemainingEnemiesForWave;

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int TimeToWaveAfterEnemiesKilled;
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

	UPROPERTY(EditAnywhere, Category="Bounds")
	UBoxComponent* ManagerBounds;

	void AddEnemy(AEnemyCharacter* Enemy);

	void RemoveEnemy(AEnemyCharacter* Enemy);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemies")
	int NumActiveEnemies = 0;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<FEnemyWave> Waves;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<ASpawnPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, Category="Combat Triggers")
	TArray<ACombatTrigger*> CombatTriggers;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCombatBegin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCombatEnd();

	UFUNCTION(BlueprintCallable)
	void StartCombat();

private:
	UPROPERTY(ReplicatedUsing=OnRep_CombatStarted)
	bool bCombatStarted = false;

	UPROPERTY(ReplicatedUsing=OnRep_CombatEnded)
	bool bCombatEnded = false;

	bool bWaitingForWave = false;

	FTimerHandle WaveWaitTimerHandle;

	UFUNCTION()
	void OnRep_CombatStarted();

	UFUNCTION()
	void OnRep_CombatEnded();
	
	void HandleSpawn();
	
	UPROPERTY(VisibleAnywhere, Category="Enemies")
	TArray<ACharacter*> Enemies;

	UPROPERTY(VisibleAnywhere, Category="Spawn")
	TArray<FEnemyWave> WavesQueue;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
