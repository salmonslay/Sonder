// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_MoveToJumpPoint.h"

#include "AIController.h"
#include "EnemyJumpPoint.h"
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
		UGameplayStatics::GetAllActorsOfClass(this, AEnemyJumpPoint::StaticClass(), TempArray);

		for(const auto JumpPoint : TempArray)
		{
			JumpPoints.Add(Cast<AEnemyJumpPoint>(JumpPoint)->Location);  
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

	OwnerLocation = Owner->GetActorLocation();
	const FVector CurrentTarget = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKeyCurrentTarget.SelectedKeyName); 

	TArray<FVector> PossiblePoints; 
	for(auto JumpPointLocation : JumpPoints)
	{
		if(JumpPointLocation.Equals(OwnerLocation, 30))
			continue; 
		
		if(FMath::Abs(JumpPointLocation.Z - OwnerLocation.Z) > MaxHeightDiff)
			continue;

		if(!Owner->HasNavigationToTarget(JumpPointLocation))
			continue; 

		PossiblePoints.Add(JumpPointLocation); 
	}

	if(PossiblePoints.IsEmpty())
		return FVector::ZeroVector;
	
	FVector ClosestPoint = PossiblePoints[0];
	float ClosestDist = FVector::Dist(ClosestPoint, CurrentTarget); 
	for(int i = 1; i < PossiblePoints.Num(); i++)
	{
		if(FVector::Dist(PossiblePoints[i], CurrentTarget) < ClosestDist)
		{
			ClosestDist = FVector::Dist(PossiblePoints[i], CurrentTarget);
			ClosestPoint = PossiblePoints[i]; 
		}
	}

	return ClosestPoint; 
}
