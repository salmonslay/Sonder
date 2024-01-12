// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemyAIController.h"

#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void AFlyingEnemyAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AFlyingEnemyAIController::Initialize()
{
	Super::Initialize();

	GetWorldTimerManager().SetTimer(RepositioningResetTimerHandle, this, &AFlyingEnemyAIController::StopRepositioning, 1.f, true);
}

void AFlyingEnemyAIController::StopRepositioning()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("bIsRepositioning"), false);
}
