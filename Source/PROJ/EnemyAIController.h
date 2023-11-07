// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class APROJCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()


	


public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TempMesh;

	void Initialize();

	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BT = nullptr;

	virtual void BeginPlay() override;

private:
	APROJCharacter* P1 = nullptr;
	APROJCharacter* P2 = nullptr;
};
