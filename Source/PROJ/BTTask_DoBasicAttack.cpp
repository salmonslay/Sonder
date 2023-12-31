// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoBasicAttack.h"

#include "AIController.h"
#include "BasicAttackComponent.h"

UBTTask_DoBasicAttack::UBTTask_DoBasicAttack()
{
	NodeName = TEXT("DoBasicAttack"); 
}

EBTNodeResult::Type UBTTask_DoBasicAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(!BasicAttackComp)
		BasicAttackComp = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<UBasicAttackComponent>(); 

	if(BasicAttackComp)
	{
		BasicAttackComp->Attack();
		return EBTNodeResult::Succeeded; 
	}
	
	return EBTNodeResult::Failed; 
}
