// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StopLaserAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_StopLaserAttack::UBTTask_StopLaserAttack()
{
	NodeName = TEXT("StopLaserAttack");
}

void UBTTask_StopLaserAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_StopLaserAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = 1;

	if (OwnerComp.GetAIOwner() == nullptr) 	return EBTNodeResult::Failed; 

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return EBTNodeResult::Failed;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsFloat("StopAttackTime", OwnerCharacter->UnlockRotationAfterAttackDuration);

	OwnerCharacter->bSetFocusToPlayer = true;
	return EBTNodeResult::Succeeded;
}
