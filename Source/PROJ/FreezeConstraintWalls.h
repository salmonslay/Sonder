// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FreezeConstraintWalls.generated.h"

class UBoxComponent;
class USphereComponent;
class ACharactersCamera;
UCLASS()

class PROJ_API AFreezeConstraintWalls : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFreezeConstraintWalls();

	UFUNCTION(BlueprintCallable)
	void MoveWallsToLoc();
	
	UFUNCTION(BlueprintCallable)
	void RemoveWallsFromLoc();
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* TriggerArea;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* RightWallLocation;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* LeftWallLocation;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:

	UPROPERTY()
	ACharactersCamera* CharactersCamera;

	
	
};
