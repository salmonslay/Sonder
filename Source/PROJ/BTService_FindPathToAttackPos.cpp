// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindPathToAttackPos.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "Grid.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_FindPathToAttackPos::UBTService_FindPathToAttackPos()
{
	NodeName = TEXT("FindPathToAttackPosition");
}

void UBTService_FindPathToAttackPos::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_FindPathToAttackPos::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_FindPathToAttackPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
	
	CurrentTargetLocation = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector("StartAttackPosition");


	OwnerGrid->TargetLocation = CurrentTargetLocation;
	OwnerGrid->StartLocation = OwnerLocation;
	Path = OwnerGrid->RequestPath(OwnerLocation, CurrentTargetLocation, bDebug);

	if (Path.IsEmpty())
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path is empty to start attack position"));
		}
		//CurrentTargetLocation = OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("StartAttackPosition");
	}
	else
	{
		OwnerCharacter->CurrentPath = Path;
	}
}
