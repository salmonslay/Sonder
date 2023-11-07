// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UBaseHealthComponent;
class UBehaviorTree;

UCLASS()
class PROJ_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle InitializerTimerHandle;

	void InitializeController();


private:
	bool bIsControllerInitialized = false;

	void KillMe();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category=Health)
	UBaseHealthComponent* HealthComponent = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTakenDamageEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathEvent();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageToPlayer = 0.f;

};
