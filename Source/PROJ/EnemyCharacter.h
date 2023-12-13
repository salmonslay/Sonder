// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class APROJCharacter;
class AGrid;
class ACombatManager;
class UEnemyHealthComponent;
class UBaseHealthComponent;
class UBehaviorTree;

UCLASS()
class PROJ_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();
	
	/** Property replication */
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeControllerFromManager();

	bool bIsControllerInitialized = false;

	void SetGridPointer(AGrid* GridPointer);

	AGrid* GetGridPointer() const;

	bool GetHasBeenAttacked() const;

	APROJCharacter* GetLatestDamageCauser();

	

	void KillMe();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle InitializerTimerHandle;

	void InitializeController();


private:

	APROJCharacter* LatestDamageCauser;

	bool bHasBeenAttacked = false;

	

	UPROPERTY()
	AGrid* PathfindingGrid;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category=Health, Replicated, BlueprintReadOnly)
	UEnemyHealthComponent* EnemyHealthComponent = nullptr;

	void CheckIfOverlappingWithGrid();
	
	/** Event called when enemy has taken damage*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakenDamageEvent(float DamageTaken);

	UFUNCTION(BlueprintImplementableEvent)
	void OnChargingAttackEvent(const float ChargingDuration);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPerformAttackEvent(const float AttackDuration);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStopAttackEvent(const float StopAttackDuration);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStunnedEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnstunnedEvent();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Stunned)
	bool bIsStunned = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_ChargingAttack)
	bool bIsChargingAttack = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Attack)
	bool bIsAttacking = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	bool bIsIdle = false;
	
	FTimerHandle StunnedTimerHandle;

	FTimerHandle ChargeAttackTimerHandle;

	FTimerHandle AttackTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ChargeAttackDuration = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PerformAttackDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float UnlockRotationAfterAttackDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StunnedDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StaggeredDuration = 0.3f;

	UPROPERTY(EditDefaultsOnly)
	float StaggeredThreshold = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float RepositioningDuration = 1.f;
	
	UFUNCTION()
	void OnRep_Stunned();

	UFUNCTION()
	void OnRep_ChargingAttack();

	UFUNCTION()
	void OnRep_Attack();
	
	void Stun(const float Duration);

	virtual void ChargeAttack();

	virtual void Attack();

	virtual void Idle();

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageToPlayer = 0.f;

	UPROPERTY(BlueprintReadOnly)
	ACombatManager* Manager;

	UPROPERTY(BlueprintReadOnly)
	FVector SpawnPosition = FVector::ZeroVector;

};
