// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ToggleChargeSpecialAttack.h"

#include "AIController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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

	UNiagaraComponent* ChargeEffectComp = Cast<UNiagaraComponent>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKeyChargingAttackEffectComp.SelectedKeyName));

	// First time charging, create the effect 
	if(!ChargeEffectComp)
	{
		ChargeEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ChargeEffect, OwnerComp.GetAIOwner()->GetPawn()->GetRootComponent(),
			NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);

		OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKeyChargingAttackEffectComp.SelectedKeyName, ChargeEffectComp); 
		
	} 

	// Toggle the effect 
	bActivateCharge ? ChargeEffectComp->Activate(true) : ChargeEffectComp->Deactivate(); 
	
	return EBTNodeResult::Succeeded; 
}
