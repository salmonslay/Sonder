// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "PlayerBasicAttack.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_API UPlayerBasicAttack : public UBoxComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UPlayerBasicAttack();

	/** Called to connect input to attack function */
	void SetUpInput(UEnhancedInputComponent* InputComp);

protected:

	virtual void BeginPlay() override;

private:

	/** Function called when player presses attack button */
	void Attack();

	UPROPERTY(EditAnywhere)
	class UInputAction* AttackInputAction; 

	/** Damage inflicted on enemies */
	UPROPERTY(EditAnywhere)
	float Damage = 5.f;

	/** How long time needs to pass between attacks */
	UPROPERTY(EditAnywhere)
	float AttackCooldown = 0.5f; 

	bool bCanAttack = true;

	void EnableCanAttack();

	UPROPERTY()
	class APROJCharacter* Player; 
	
};
