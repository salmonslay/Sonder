// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "BehaviorTree/BehaviorTree.h"

void AEnemyAIController::Initialize()
{
	if(BT)
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviourTReetorun %s"), *BT->GetName());
		RunBehaviorTree(BT);
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}
