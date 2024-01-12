// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyJumpPoint.generated.h"

class AEnemyJumpTrigger;
class UBoxComponent;

UCLASS()
class SONDER_API AEnemyJumpPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyJumpPoint();

	UPROPERTY(EditAnywhere)
	UBoxComponent* Point;

	FVector Location = FVector::ZeroVector;

	TWeakObjectPtr<AEnemyJumpTrigger> JumpTrigger = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
