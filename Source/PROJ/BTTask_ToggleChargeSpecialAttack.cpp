// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ToggleChargeSpecialAttack.h"

#include "AIController.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ToggleChargeSpecialAttack::UBTTask_ToggleChargeSpecialAttack()
{
	NodeName = TEXT("Special Attack Toggle"); 
}

void UBTTask_ToggleChargeSpecialAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_ToggleChargeSpecialAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory); 

	// Set we are charging the attack BB Key 
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyChargingSpecialAttack.SelectedKeyName, bActivateCharge); 

	Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetPawn())->ServerRPC_ToggleChargeEffect(bActivateCharge); 
	
	return EBTNodeResult::Succeeded; 
}
