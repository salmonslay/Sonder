// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_MoveToJumpPoint.h"

#include "AIController.h"
#include "EnemyJumpTrigger.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_MoveToJumpPoint::UBTService_MoveToJumpPoint()
{
	NodeName = TEXT("MoveToJumpPoint"); 
}

EBTNodeResult::Type UBTService_MoveToJumpPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if(!IsValid(&OwnerComp))
		return Result; 

	if(JumpPoints.IsEmpty())
	{
		TArray<AActor*> TempArray; 
		UGameplayStatics::GetAllActorsOfClass(this, AEnemyJumpTrigger::StaticClass(), TempArray);

		for(const auto JumpPoint : TempArray)
		{
			JumpPoints.Add(Cast<AEnemyJumpTrigger>(JumpPoint)->JumpPoint1);  
			JumpPoints.Add(Cast<AEnemyJumpTrigger>(JumpPoint)->JumpPoint2);  
		}
	}
		
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, GetJumpPoint(OwnerComp));

	return Result; 
}

FVector UBTService_MoveToJumpPoint::GetJumpPoint(const UBehaviorTreeComponent& OwnerComp)
{
	const AShadowCharacter* Owner = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if(!Owner)
		return FVector::ZeroVector; 

	const FVector CurrentTarget = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKeyCurrentTarget.SelectedKeyName); 

	TArray<UBoxComponent*> PossiblePoints; 
	for(auto JumpPoint : JumpPoints)
	{
		if(JumpPoint->GetComponentLocation().Equals(Owner->GetActorLocation(), 10))
			continue; 
		
		if(FMath::Abs(JumpPoint->GetComponentLocation().Z - Owner->GetActorLocation().Z) > MaxHeightDiff)
			continue;

		if(!Owner->HasNavigationToTarget(JumpPoint->GetComponentLocation()))
			continue; 

		PossiblePoints.Add(JumpPoint); 
	}

	// float DistToPoint = 0;
	// if(const auto NearestActor = UGameplayStatics::FindNearestActor(CurrentTarget, PossiblePoints, DistToPoint))
	// 	return NearestActor->GetActorLocation();

	if(PossiblePoints.IsEmpty())
		return FVector::ZeroVector;
	
	FVector ClosestPoint = PossiblePoints[0]->GetComponentLocation();
	float ClosestDist = FVector::Dist(ClosestPoint, CurrentTarget); 
	for(int i = 1; i < PossiblePoints.Num(); i++)
	{
		if(FVector::Dist(PossiblePoints[i]->GetComponentLocation(), CurrentTarget) < ClosestDist)
		{
			ClosestDist = FVector::Dist(PossiblePoints[i]->GetComponentLocation(), CurrentTarget);
			ClosestPoint = PossiblePoints[i]->GetComponentLocation(); 
		}
	}

	return ClosestPoint; 
}
