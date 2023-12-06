// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Dash.h"

#include "AIController.h"
#include "SoulBaseStateNew.h"

void UBTTask_Dash::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
	
}

EBTNodeResult::Type UBTTask_Dash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(const auto SoulBaseState = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<USoulBaseStateNew>())
	{
		SoulBaseState->Dash(); 
		return EBTNodeResult::Succeeded; 
	}

	return EBTNodeResult::Failed; 
}
