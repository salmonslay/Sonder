// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsStunned.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_IsStunned::UBTService_IsStunned()
{
	NodeName = TEXT("IsStunnded");
}

void UBTService_IsStunned::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_IsStunned::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_IsStunned::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (OwnerComp.GetAIOwner() == nullptr) return;

	TreeComponent = &OwnerComp;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsStunned", OwnerCharacter->bIsStunned);
	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsFloat("StunnedDuration", OwnerCharacter->StunnedDuration);

	/*
	if (OwnerCharacter->bIsStunned && OwnerCharacter->StunnedDuration > OwnerCharacter->StaggeredDuration &&
		!OwnerComp.GetBlackboardComponent()->GetValueAsBool("bIsRepositioning"))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsRepositioning", true);
		GetWorld()->GetTimerManager().SetTimer(StopRepositioningTimerHandle, this, &UBTService_IsStunned::StopRepositioning,
			OwnerCharacter->RepositioningDuration + OwnerCharacter->StunnedDuration, false);
	}
	*/
}

void UBTService_IsStunned::StopRepositioning()
{

	TreeComponent->GetBlackboardComponent()->SetValueAsBool("bIsRepositioning", false);
	TreeComponent->GetBlackboardComponent()->ClearValue("bIsRepositioning");

	GetWorld()->GetTimerManager().ClearTimer(StopRepositioningTimerHandle);
}
