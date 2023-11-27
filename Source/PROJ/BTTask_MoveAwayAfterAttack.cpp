// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveAwayAfterAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"

UBTTask_MoveAwayAfterAttack::UBTTask_MoveAwayAfterAttack()
{
	NodeName = TEXT("MoveAwayAfterAttack");
}

void UBTTask_MoveAwayAfterAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_MoveAwayAfterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// generate new point within attack radius from player
	// calculate path 
	
	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_MoveAwayAfterAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerCharacter->Idle();

	OwnerLocation = OwnerCharacter->GetActorLocation();

	if (!OwnerCharacter->IsPathValid())
	{
		return;
	}

	// Check if the entity has reached the current waypoint.
	if (OwnerCharacter->CurrentPath.IsEmpty()) 
	{
		return;
	}
	
}
