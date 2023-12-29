// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "FlyingEnemyCharacter.generated.h"

class USpotLightComponent;
/**
 * 
 */
UCLASS()
class PROJ_API AFlyingEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	void SetPointerToPath(const TArray<FVector>* PathPointer);

	bool IsPathValid() const;
	
	TArray<FVector> CurrentPath = TArray<FVector>();

	// ==== laser stuff 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpotLightComponent* Laser;

	UPROPERTY()
	USceneComponent* LaserEndComponent;
	
	/** Laser range*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaserRange = 600.f;

	/** Max difference in height the enemy can be compared to player before being able to attack*/
	UPROPERTY(EditAnywhere)
	float MaxAttackHeightDifference = 40.f;

	UPROPERTY(EditAnywhere)
	float MaxAngleToAttack = 30.f;

	virtual void Tick(float DeltaSeconds) override;

	bool bSetFocusToPlayer = true;

	UPROPERTY()
	ACharacter* CurrentTargetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* LaserBeam;
};
