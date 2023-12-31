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

	virtual void Initialize();

	APROJCharacter* GetPlayerFromController(int Index) const;
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BT = nullptr;

private:

	UPROPERTY()
	APROJCharacter* P1 = nullptr;

	UPROPERTY()
	APROJCharacter* P2 = nullptr;
};
