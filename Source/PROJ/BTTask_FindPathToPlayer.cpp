// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPathToPlayer.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "Grid.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

class APROJCharacter;

UBTTask_FindPathToPlayer::UBTTask_FindPathToPlayer()
{
	NodeName = TEXT("FindPathToPlayer");
}

void UBTTask_FindPathToPlayer::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_FindPathToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_FindPathToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	
	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	OwnerGrid = OwnerCharacter->GetGridPointer();

	if (OwnerGrid == nullptr) return;
	
	APROJCharacter* PlayerToAttack;
	UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("PlayerToAttack");
	if (PlayerObject)
	{
		PlayerToAttack = Cast<APROJCharacter>(PlayerObject);
		if (PlayerToAttack)
		{
			CurrentTargetLocation = PlayerToAttack->GetActorLocation();
		}
	}

	Path = OwnerGrid->RequestPath(OwnerLocation, CurrentTargetLocation, bDebug);
	
}





