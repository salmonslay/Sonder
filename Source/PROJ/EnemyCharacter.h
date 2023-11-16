// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

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
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle InitializerTimerHandle;

	void InitializeController();


private:

	void KillMe();

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
	void OnBasicAttackEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathEvent();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageToPlayer = 0.f;

	UPROPERTY(BlueprintReadOnly)
	ACombatManager* Manager;

};
