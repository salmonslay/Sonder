// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_API UBaseHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;
	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);
	UFUNCTION()
	void SetMaxHealth(float NewMaxHealth);
	UFUNCTION(BlueprintCallable)
	void RefillHealth();


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Health, meta=(AllowPrivateAccess = true), SaveGame)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health, meta=(AllowPrivateAccess = true), SaveGame)
	float MaxHealth = 100;
		
};
