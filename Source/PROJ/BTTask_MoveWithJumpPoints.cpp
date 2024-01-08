// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveWithJumpPoints.h"

#include "AIController.h"
#include "EnemyJumpPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MoveWithJumpPoints::UBTTask_MoveWithJumpPoints()
{
	NodeName = TEXT("MoveToWithJumpPoints");
}

EBTNodeResult::Type UBTTask_MoveWithJumpPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!IsValid(&OwnerComp))
	{
		return Result;
	}

	BlackboardComponent = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	
	if (BlackboardComponent == nullptr)
	{
		return Result;
	}

	if(JumpPoints.IsEmpty() || JumpPointLocations.IsEmpty())
	{
		TArray<AActor*> TempArray; 
		UGameplayStatics::GetAllActorsOfClass(this, JumpPointClass, TempArray);

		for(const auto JumpPoint : TempArray)
		{
			JumpPoints.Add(Cast<AEnemyJumpPoint>(JumpPoint));
			JumpPointLocations.Add(Cast<AEnemyJumpPoint>(JumpPoint)->GetActorLocation());
		}
	}

	GetJumpPointPath(OwnerComp);

	if (!JumpPointPath.IsEmpty())
	{
		
	}

	return Result;
}

void UBTTask_MoveWithJumpPoints::GetJumpPointPath(const UBehaviorTreeComponent& OwnerComp)
{
}
