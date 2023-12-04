// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Dash.h"

#include "AIController.h"
#include "ShadowSoulCharacter.h"
#include "SoulDashingState.h"

void UBTTask_Dash::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
	
}

EBTNodeResult::Type UBTTask_Dash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(const auto ShadowSoul = Cast<AShadowSoulCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		ShadowSoul->SwitchState(ShadowSoul->DashState);
		return EBTNodeResult::Succeeded; 
	}

	return EBTNodeResult::Failed; 
}
