// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlateBase.generated.h"

class AMovingPlatform;
class UBoxComponent;

UCLASS()
class PROJ_API APressurePlateBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressurePlateBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerArea = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PressurePlateMesh = nullptr;
	
};
