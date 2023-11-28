// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindPath.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "Grid.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_FindPath::UBTService_FindPath()
{
	NodeName = TEXT("FindPath");
}

void UBTService_FindPath::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_FindPath::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_FindPath::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	if(OwnerComp.GetBlackboardComponent()->GetValueAsBool("bIsRepositioning"))
	{
		Path = OwnerGrid->RequestPath(OwnerLocation, OwnerComp.GetBlackboardComponent()->GetValueAsVector("SpawnPosition"), bDebug);
		
	}
	else if (OwnerComp.GetBlackboardComponent()->GetValueAsBool("bFoundPlayerWithinAttackRadius") &&
		!OwnerComp.GetBlackboardComponent()->GetValueAsBool("bIsInRangeToAttack"))
	{
		Path = OwnerGrid->RequestPath(OwnerLocation, OwnerComp.GetBlackboardComponent()->GetValueAsVector("StartAttackPosition"), bDebug);
	}
	else
	{
		UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("CurrentTargetPlayer");

		if (PlayerObject == nullptr)
		{
			return;
		}
		if (const APROJCharacter* PlayerToAttack = Cast<APROJCharacter>(PlayerObject))
		{
			CurrentTargetLocation = PlayerToAttack->GetActorLocation();
		}
		
		Path = OwnerGrid->RequestPath(OwnerLocation, CurrentTargetLocation, bDebug);
	}
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
