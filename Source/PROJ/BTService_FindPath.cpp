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

	BlackboardComponent = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	
	if (BlackboardComponent == nullptr)
	{
		return;
	}

	if(BlackboardComponent->GetValueAsBool("bIsRepositioning"))
	{
		BlackboardComponent->SetValueAsVector(TEXT("RetreatLocation"), OwnerCharacter->RetreatLocation);
		Path = OwnerGrid->RequestPath(OwnerLocation, BlackboardComponent->GetValueAsVector("RetreatLocation"), bDebug);
		
	}
	else if (BlackboardComponent ->GetValueAsBool("bFoundPlayerWithinAttackRadius") &&
		!BlackboardComponent ->GetValueAsBool("bIsInRangeToAttack"))
	{
		Path = OwnerGrid->RequestPath(OwnerLocation, BlackboardComponent->GetValueAsVector("StartAttackPosition"), bDebug);
	}
	else
	{
		UObject* PlayerObject = BlackboardComponent->GetValueAsObject("CurrentTargetPlayer");

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
