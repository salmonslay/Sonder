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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpotLightComponent* Laser;

	UPROPERTY()
	USceneComponent* LaserEndComponent;

	FVector LaserPointerDestination = FVector::ZeroVector;
	
	
	
};
