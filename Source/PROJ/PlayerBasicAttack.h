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

private:

	/** Function called when player presses attack button */
	void Attack();

	UPROPERTY(EditAnywhere)
	class UInputAction* AttackInputAction; 

	UPROPERTY(EditAnywhere)
	float Damage = 5.f; 
};
