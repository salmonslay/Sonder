// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToMovingPlatform.h"

#include "AIController.h"
#include "MovingPlatform.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MoveToMovingPlatform::UBTTask_MoveToMovingPlatform()
{
	NodeName = TEXT("MoveToMovingPlatform");
}

EBTNodeResult::Type UBTTask_MoveToMovingPlatform::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if(MovingPlatforms.IsEmpty())
	{
		TArray<AActor*> TempArray; 
		UGameplayStatics::GetAllActorsOfClass(this, AMovingPlatform::StaticClass(), TempArray);

		for(const auto Platform : TempArray)
		{
			MovingPlatforms.Add(Cast<AMovingPlatform>(Platform)->GetActorLocation());  
		}
	}

	// TODO: Check if tree needs balancing, might have to about all other move-to-tasks when enemy is missing line of sight
	// TODO: Check exactly which heights work to check if is on same level as player
		
	BlackboardComponent->SetValueAsVector(BlackboardKey.SelectedKeyName, GetMovingPlatform(OwnerComp));
	return Result;
}

FVector UBTTask_MoveToMovingPlatform::GetMovingPlatform(const UBehaviorTreeComponent& OwnerComp)
{
	const AShadowCharacter* Owner = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if(!Owner)
	{
		return FVector::ZeroVector; 
	}

	OwnerLocation = Owner->GetActorLocation();
	const FVector CurrentTarget = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKeyCurrentTarget.SelectedKeyName); 

	TArray<FVector> PossiblePoints; 
	for(auto MovingPlatformLoc : MovingPlatforms)
	{
		// maybe check if really close to platform?
		PossiblePoints.Add(MovingPlatformLoc); 
	}
	
	if(PossiblePoints.IsEmpty())
		return FVector::ZeroVector;

	FVector ClosestPoint = PossiblePoints[0];
	FVector LocationToCheck;

	// if is in approximately same height as current move target, choose the moving platform that is closest to enemy
	if(FMath::Abs(OwnerLocation.Z - CurrentTarget.Z) <= MaxHeightDifferenceToMarkAsSameHeight)
	{
		LocationToCheck = OwnerLocation;
	}
	// if not, choose the moving platform that is closest to current target player location
	else
	{
		LocationToCheck = CurrentTarget;
	}
	float ClosestDist = FVector::Dist(ClosestPoint, LocationToCheck); 
	for(int i = 1; i < PossiblePoints.Num(); i++)
	{
		if(FVector::Dist(PossiblePoints[i], LocationToCheck) < ClosestDist)
		{
			ClosestDist = FVector::Dist(PossiblePoints[i], LocationToCheck);
			ClosestPoint = PossiblePoints[i]; 
		}
	}
	return ClosestPoint; 
}
