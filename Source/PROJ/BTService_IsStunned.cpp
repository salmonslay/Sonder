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

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsStunned", OwnerCharacter->bIsStunned);
	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsFloat("StunnedDuration", OwnerCharacter->StunnedDuration);


	if (OwnerCharacter->bIsStunned)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Is stunned"));	
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT(" NOT stunned"));	

	}
}
