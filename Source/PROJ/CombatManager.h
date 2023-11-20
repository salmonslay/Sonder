// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpawnPoint.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"


class ACombatTriggeredBase;
class ACombatTrigger;
class AEnemyCharacter;
class UBoxComponent;
class ASpawnPoint;

USTRUCT(BlueprintType, Category = "Enemy Spawn Wave")
struct FEnemyWave
{
	GENERATED_BODY()

	//The class of enemy to spawn, use the BP
	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	//An array of the spawn points to use for that wave, used in order, looping if necessary
	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	TArray<ASpawnPoint*> SpawnPoints;

	//Number of enemies to spawn this wave
	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int NumEnemies;

	//Allowed number of remaining enemies in the scene for the wave to spawn
	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int AllowedRemainingEnemiesForWave;

	//Time to spawn once the threshold has been met
	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	int TimeToWaveAfterEnemiesKilled;

	UPROPERTY(EditAnywhere, Category="Enemy Spawn Wave")
	ACombatTriggeredBase* WaveStartedTriggeredActor;
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

	//Bounds of the actor intended to use in overlaps, currently unused
	UPROPERTY(EditAnywhere, Category="Bounds")
	UBoxComponent* ManagerBounds;

	//Adds an enemy to the array of managed enemies
	void AddEnemy(AEnemyCharacter* Enemy);

	//Removes an enemy from the array of managed enemies and decrements the NumActiveEnemies counter
	//When NumActiveEnemies is 0 and the WavesQueue is empty it prompts the OnCombatEnd event
	UFUNCTION(BlueprintCallable)
	void RemoveEnemy(AEnemyCharacter* Enemy);

	//Used to keep track of the number of active enemies in the combat scenario
	//Used instead Enemies.Num() because it needs to be increased as the spawns are queued and not when they are instanced
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemies")
	int NumActiveEnemies = 0;

	UPROPERTY(EditAnywhere, Category="TriggeredActors")
	ACombatTriggeredBase* StartCombatTriggeredActor;

	UPROPERTY(EditAnywhere, Category="TriggeredActors")
	ACombatTriggeredBase* EndCombatTriggeredActor;

	//Array of all the waves to spawn, set in editor
	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<FEnemyWave> Waves;

	//Array of all the spawn points used by the manager, needed for hooking the spawn points to the manager
	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<ASpawnPoint*> SpawnPoints;

	//Array of all the combat triggers used to trigger this manager's combat
	UPROPERTY(EditAnywhere, Category="Combat Triggers")
	TArray<ACombatTrigger*> CombatTriggers;

	//Plays on server and client, use for sound and visuals when combat starts
	UFUNCTION(BlueprintImplementableEvent)
	void OnCombatBegin();

	//Plays on server and client, use for sound and visuals when combat ends
	UFUNCTION(BlueprintImplementableEvent)
	void OnCombatEnd();

	//The function called by the combat triggers, starts the spawn sequence and plays the OnCombatStart event
	UFUNCTION(BlueprintCallable)
	void StartCombat();

private:
	//Used for networking the start and end events and for checking whether to spawn enemies 
	UPROPERTY(ReplicatedUsing=OnRep_CombatStarted)
	bool bCombatStarted = false;
	UPROPERTY(ReplicatedUsing=OnRep_CombatEnded)
	bool bCombatEnded = false;

	//Used to prevent timed spawned events from overwriting themselves every tick
	bool bWaitingForWave = false;

	//Timer handle used for timed wave spawns
	FTimerHandle WaveWaitTimerHandle;

	//Allows the client to perform the combat start and end events
	UFUNCTION()
	void OnRep_CombatStarted();
	UFUNCTION()
	void OnRep_CombatEnded();

	//Pops the first element of the WavesQueue and distributes necessary information to the relevant spawn points
	void HandleSpawn();

	//Array of all the enemies tracked by the manager
	UPROPERTY(VisibleAnywhere, Category="Enemies")
	TArray<ACharacter*> Enemies;

	//A queue of all the waves to spawn, copies the array
	UPROPERTY(VisibleAnywhere, Category="Spawn")
	TArray<FEnemyWave> WavesQueue;

	//I did some testing on events triggered by properties being changed in the editor
	//I failed to make something useful in the allotted time so I removed it
	//Now the code fails to compile if this isn't included
	//DO NOT REMOVE
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
