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
		UGameplayStatics::GetAllActorsOfClass(this, PlatformClass, TempArray);

		for(const auto Platform : TempArray)
		{
			MovingPlatforms.Add(Cast<AMovingPlatform>(Platform)->GetActorLocation());  
		}
	}

	// TODO: Check if tree needs balancing, might have to about all other move-to-tasks when enemy is missing line of sight
	// TODO: Check exactly which heights work to check if is on same level as player


	BlackboardComponent->SetValueAsVector(BlackboardKey.SelectedKeyName, FVector(OwnerLocation.X, GetMovingPlatform(OwnerComp).Y + 20, OwnerLocation.Z ));
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), BlackboardComponent->GetValueAsVector(BlackboardKey.SelectedKeyName), 30.f, 30, FColor::Green, false, 5.f );
	}
		
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
	if(OwnerLocation.Z >= CurrentTarget.Z)
	{
		LocationToCheck = OwnerLocation;
	}
	// if not, choose the moving platform that is closest to current target player location
	else
	{
		LocationToCheck = CurrentTarget;
	}

	// choose the platform loc that is closest to enemy´s Z loc, if two platforms have the same height choose the platform closest to enemy´s Y value
	float ClosestDist= FVector::Dist(ClosestPoint, LocationToCheck);
	float ClosestDistZ = ClosestPoint.Z - LocationToCheck.Z;
	float ClosestDistY = ClosestPoint.Y - LocationToCheck.Y;
	
	for(int i = 1; i < PossiblePoints.Num(); i++)
	{
		if (FMath::Abs(PossiblePoints[i].Z - LocationToCheck.Z) < ClosestDistZ)
		{
			ClosestDistZ = PossiblePoints[i].Z - LocationToCheck.Z;
			ClosestPoint = PossiblePoints[i];
		}
		else if (FMath::IsNearlyEqual(PossiblePoints[i].Z - LocationToCheck.Z, ClosestDistZ, 5))
		{
			if (FMath::Abs(PossiblePoints[i].Y - LocationToCheck.Y) < ClosestDistY)
			{
				ClosestDistY = PossiblePoints[i].Y - LocationToCheck.Y;
				ClosestPoint = PossiblePoints[i];
			}
		}
		/*
		if(FVector::Dist(PossiblePoints[i], LocationToCheck) < ClosestDist)
		{
			ClosestDist = FVector::Dist(PossiblePoints[i], LocationToCheck);
			ClosestPoint = PossiblePoints[i]; 
		}*/
	}
	return ClosestPoint; 
}
