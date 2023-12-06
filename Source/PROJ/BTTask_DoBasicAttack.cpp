// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoBasicAttack.h"

#include "AIController.h"
#include "PlayerBasicAttack.h"

UBTTask_DoBasicAttack::UBTTask_DoBasicAttack()
{
	NodeName = TEXT("DoBasicAttack"); 
}

void UBTTask_DoBasicAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_DoBasicAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(const auto BasicAttackComp = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<UPlayerBasicAttack>())
	{
		BasicAttackComp->Attack();
		return EBTNodeResult::Succeeded; 
	}
	
	return EBTNodeResult::Failed; 
}
