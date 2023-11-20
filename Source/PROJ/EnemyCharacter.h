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

	UPROPERTY(EditDefaultsOnly)
	float StaggeredThreshold = 0.f;

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

	void CheckIfOverlappingWithGrid();

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category=Health, Replicated)
	UEnemyHealthComponent* EnemyHealthComponent = nullptr;

	/** Event called when enemy has taken damage*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakenDamageEvent(float DamageTaken);

	UFUNCTION(BlueprintImplementableEvent)
	void OnChargingAttackEvent(const float ChargingDuration);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPerformAttackEvent(const float AttackDuration);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStunnedEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnstunnedEvent();
	
	UPROPERTY(ReplicatedUsing=OnRep_Stunned)
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing=OnRep_ChargingAttack)
	bool bIsChargingAttack = false;

	UPROPERTY(ReplicatedUsing=OnRep_Attack)
	bool bIsAttacking = false;

	UPROPERTY(Replicated)
	bool bIsIdle = false;
	
	FTimerHandle StunnedTimerHandle;

	FTimerHandle ChargeAttackTimerHandle;

	FTimerHandle AttackTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float ChargeAttackDuration = 0.7f;

	UPROPERTY(EditDefaultsOnly)
	float PerformAttackDuration = 0.5f;
	
	UFUNCTION()
	void OnRep_Stunned();

	UFUNCTION()
	void OnRep_ChargingAttack();

	UFUNCTION()
	void OnRep_Attack();
	
	void Stun(const float Duration);

	void ChargeAttack();

	virtual void Attack();

	void Idle();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageToPlayer = 0.f;

	UPROPERTY(BlueprintReadOnly)
	ACombatManager* Manager;

};
