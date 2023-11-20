// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FlyingAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"

UBTTask_FlyingAttack::UBTTask_FlyingAttack()
{
	NodeName = TEXT("FlyingAttack");
}

void UBTTask_FlyingAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_FlyingAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// code below makes it so TickTask is called 
	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_FlyingAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	
}

void UBTTask_FlyingAttack::MovePlayerToAttackHeight()
{
}

void UBTTask_FlyingAttack::DoLaserAttack()
{
	
}
