// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Dash.h"

#include "AIController.h"
#include "ShadowSoulCharacter.h"
#include "SoulBaseStateNew.h"
#include "SoulDashingState.h"

void UBTTask_Dash::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
	
}

EBTNodeResult::Type UBTTask_Dash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Starting dash
	if(const auto SoulBaseState = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<USoulBaseStateNew>())
	{
		SoulBaseState->Dash();
		
		bNotifyTick = 1; 
		return EBTNodeResult::InProgress; 
	}
	
	return EBTNodeResult::Failed; 
}

void UBTTask_Dash::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Check if still dashing 
	if(const auto Soul = Cast<AShadowSoulCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(Soul->GetCurrentState()->IsA(USoulDashingState::StaticClass()))
			return FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);

		return FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); 
	}

	return FinishLatentTask(OwnerComp, EBTNodeResult::Failed); 
}
