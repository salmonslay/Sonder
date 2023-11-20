// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAttackPosition.h"

UBTTask_MoveToAttackPosition::UBTTask_MoveToAttackPosition()
{
	NodeName = TEXT("MoveToAttackPosition");
}

void UBTTask_MoveToAttackPosition::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_MoveToAttackPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_MoveToAttackPosition::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
