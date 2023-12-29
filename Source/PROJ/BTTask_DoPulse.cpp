// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoPulse.h"

#include "AIController.h"
#include "RobotBaseState.h"

UBTTask_DoPulse::UBTTask_DoPulse()
{
	NodeName = TEXT("DoPulse");
}

EBTNodeResult::Type UBTTask_DoPulse::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	if(!RobotBaseState)
		RobotBaseState = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<URobotBaseState>(); 

	if(RobotBaseState)
	{
		RobotBaseState->Pulse(); 
		return EBTNodeResult::Succeeded; 
	}

	return EBTNodeResult::Failed; 
}
