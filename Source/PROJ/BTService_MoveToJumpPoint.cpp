// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_MoveToJumpPoint.h"

#include "AIController.h"
#include "EnemyJumpTrigger.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_MoveToJumpPoint::UBTService_MoveToJumpPoint()
{
	NodeName = TEXT("MoveToJumpPoint"); 
}

EBTNodeResult::Type UBTService_MoveToJumpPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if(JumpPoints.IsEmpty())
		UGameplayStatics::GetAllActorsOfClass(this, AEnemyJumpTrigger::StaticClass(), JumpPoints); 
		
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, GetJumpPoint(OwnerComp));

	return Result; 
}

FVector UBTService_MoveToJumpPoint::GetJumpPoint(const UBehaviorTreeComponent& OwnerComp)
{
	const APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();

	TArray<AActor*> PossiblePoints; 
	for(auto JumpPoint : JumpPoints)
	{
		if(FMath::Abs(JumpPoint->GetActorLocation().Z - Owner->GetActorLocation().Z) > MaxHeightDiff)
			continue;

		PossiblePoints.Add(JumpPoint); 
	}

	float DistToPoint = 0;
	if(const auto NearestActor = UGameplayStatics::FindNearestActor(Owner->GetActorLocation(), PossiblePoints, DistToPoint))
		return NearestActor->GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("No available jump point"))
	return FVector::ZeroVector; 
}
