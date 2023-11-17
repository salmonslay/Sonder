// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindPathToTargetPlayer.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "FlyingEnemyCharacter.h"
#include "Grid.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_FindPathToTargetPlayer::UBTService_FindPathToTargetPlayer()
{
	NodeName = TEXT("FindPathToTargetPlayer");
}

void UBTService_FindPathToTargetPlayer::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_FindPathToTargetPlayer::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_FindPathToTargetPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	OwnerGrid = OwnerCharacter->GetGridPointer();

	if (OwnerGrid == nullptr)
	{
		return;
	}
	
	APROJCharacter* PlayerToAttack;
	UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("CurrentTargetPlayer");

	if (PlayerObject == nullptr)
	{
		return;
	}
	PlayerToAttack = Cast<APROJCharacter>(PlayerObject);
	if (PlayerToAttack)
	{
		CurrentTargetLocation = PlayerToAttack->GetActorLocation();
	}

	OwnerGrid->TargetLocation = CurrentTargetLocation;
	OwnerGrid->StartLocation = OwnerLocation;
	Path = OwnerGrid->RequestPath(OwnerLocation, CurrentTargetLocation, bDebug);

	if (Path.IsEmpty())
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path is empty"));
		}
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bHasFoundPathToTargetPlayer", false);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bHasFoundPathToTargetPlayer", true);
		OwnerCharacter->CurrentPath = Path;
	}
}
