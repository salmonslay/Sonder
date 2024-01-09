// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StopLaserAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_StopLaserAttack::UBTTask_StopLaserAttack()
{
	NodeName = TEXT("StopLaserAttack");
}

EBTNodeResult::Type UBTTask_StopLaserAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr) 	return EBTNodeResult::Failed;

	TreeComponent = &OwnerComp;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return EBTNodeResult::Failed;

	OwnerLocation = OwnerCharacter->GetActorLocation();
	
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat("StopAttackTime", OwnerCharacter->UnlockRotationAfterAttackDuration);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsRepositioning", true);
	GetWorld()->GetTimerManager().SetTimer(StopRepositioningTimerHandle, this, &UBTTask_StopLaserAttack::StopRepositioning, OwnerCharacter->RepositioningDuration, false);
	OwnerCharacter->bSetFocusToPlayer = true;
	OwnerCharacter->OnStopAttackEvent(OwnerCharacter->UnlockRotationAfterAttackDuration);
	OwnerCharacter->Idle();
	return EBTNodeResult::Succeeded;
}

void UBTTask_StopLaserAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	StopRepositioning();
}

void UBTTask_StopLaserAttack::StopRepositioning()
{
	ensure(TreeComponent != nullptr);
	ensure(TreeComponent->GetBlackboardComponent() != nullptr);
	
	TreeComponent->GetBlackboardComponent()->SetValueAsBool("bIsRepositioning", false);
	TreeComponent->GetBlackboardComponent()->ClearValue("bIsRepositioning");
	GetWorld()->GetTimerManager().ClearTimer(StopRepositioningTimerHandle);
	
}
