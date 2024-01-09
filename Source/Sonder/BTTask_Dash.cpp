// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Dash.h"

#include "AIController.h"
#include "ShadowSoulCharacter.h"
#include "SoulBaseStateNew.h"
#include "SoulDashingState.h"

EBTNodeResult::Type UBTTask_Dash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(!ShadowSoul)
		ShadowSoul = Cast<AShadowSoulCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if(ShadowSoul && !SoulBaseState)
		SoulBaseState = ShadowSoul->FindComponentByClass<USoulBaseStateNew>(); 

	// Starting dash
	if(SoulBaseState)
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
	if(ShadowSoul)
	{
		if(ShadowSoul->GetCurrentState()->IsA(USoulDashingState::StaticClass()))
			return FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);

		return FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); 
	}

	return FinishLatentTask(OwnerComp, EBTNodeResult::Failed); 
}
