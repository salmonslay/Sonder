// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveWithJumpPoints.h"

#include "AIController.h"
#include "EnemyJumpPoint.h"
#include "EnemyJumpTrigger.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MoveWithJumpPoints::UBTTask_MoveWithJumpPoints()
{
	NodeName = TEXT("MoveToWithJumpPoints");
}

EBTNodeResult::Type UBTTask_MoveWithJumpPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	if (!IsValid(&OwnerComp) || OwnerComp.GetAIOwner() == nullptr)
	{
		return Result;
	}

	OwnerCharacter = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter.Get() == nullptr)
	{
		return Result;
	}

	// if owner has not updated its location, update it
	if (OwnerLocation != OwnerCharacter->CurrentLocation)
	{
		OwnerLocation = OwnerCharacter->GetActorLocation();
		OwnerCharacter->CurrentLocation = OwnerLocation;
	}
	
	BlackboardComponent = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	
	if (BlackboardComponent == nullptr)
	{
		return Result;
	}

	CurrentTargetLocation = OwnerCharacter->CurrentTargetLocation;
	
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

	ClosestMoveLoc = GetClosestReachableJumpPointLocation();

	if (ClosestMoveLoc == FVector::ZeroVector)
	{
		ClosestMoveLoc = GetClosestJumpPointTo(CurrentTargetLocation);
	}
	BlackboardComponent->SetValueAsVector(BlackboardKey.SelectedKeyName,  FVector(OwnerLocation.X, ClosestMoveLoc.Y, OwnerLocation.Z ) );

	
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), BlackboardComponent->GetValueAsVector(BlackboardKey.SelectedKeyName), 30.f, 6, FColor::Magenta, false, 0.2f );
	}
	
	bNotifyTick = 1; 
	return Result;
}


FVector UBTTask_MoveWithJumpPoints::GetClosestReachableJumpPointLocation()
{
	if (JumpPointLocations.IsEmpty())
	{
		return FVector::ZeroVector;
	}

	FVector ClosestToPlayer = FVector::ZeroVector;
	float MinDistancePlayer = REALLY_LARGE_NUMBER;
	// TODO: the point has to be within range of the enemy, getting the closest one to the target but within range.

	// Choose the jump point location that is as close to the player as possible, but also reachable aka the same height as enemy.
	for (FVector JumpPoint : JumpPointLocations)
	{
		if (OwnerCharacter->IsLeveledWithLocation(JumpPoint) && FVector::Distance(JumpPoint, OwnerLocation) <= MaxDistanceToMarkAsReachable)
		{
			if (FVector::Distance(JumpPoint, CurrentTargetLocation) <= MinDistancePlayer)
			{
				ClosestToPlayer = JumpPoint;
				MinDistancePlayer = FVector::Distance(JumpPoint, CurrentTargetLocation);
			}
		}
	}
	return ClosestToPlayer;
}

FVector UBTTask_MoveWithJumpPoints::GetClosestJumpPointTo(const FVector& Loc)
{
	FVector ClosestToPlayer;
	float MinDistancePlayer = REALLY_LARGE_NUMBER;
	
	// Choose the jump point location that is as close to the player as possible
	for (FVector JumpPoint : JumpPointLocations)
	{
		if (FVector::Distance(JumpPoint, Loc) <= MinDistancePlayer)
		{
			ClosestToPlayer = JumpPoint;
			MinDistancePlayer = FVector::Distance(JumpPoint, Loc);
		}
	} 
	return ClosestToPlayer;
}

TWeakObjectPtr<AEnemyJumpPoint> UBTTask_MoveWithJumpPoints::GetJumpPointFromLocation(const FVector& Loc)
{
	TWeakObjectPtr<AEnemyJumpPoint> Temp;
	for (const auto JumpPoint : JumpPoints)
	{
		if (JumpPoint->GetActorLocation() == Loc)
		{
			Temp = JumpPoint;
		}
	}
	if (Temp.Get() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No jumppoint found with location, something is severely wrong"));
	}
	return Temp;
}

void UBTTask_MoveWithJumpPoints::GetJumpPointPath(const FVector& ClosestPointLoc)
{
	TArray<FVector> TempPath;
	TWeakObjectPtr<AEnemyJumpPoint> ClosestJumpPoint = GetJumpPointFromLocation(ClosestPointLoc);

	// Iterate jump points from the closest one, checking its connected jump points
	//TODO: if player is kinda the same height, check for y value closest to. If not, try to find the one closest to z value
	float MinDistanceToPlayer = REALLY_LARGE_NUMBER;
	TWeakObjectPtr<AEnemyJumpPoint> MostAccurateJumpPoint;
	for (const auto JumpPoint : ClosestJumpPoint->JumpTrigger.Get()->JumpPoints)
	{
		if (FVector::Distance(JumpPoint->GetActorLocation(), CurrentTargetLocation) <= MinDistanceToPlayer)
		{
			MostAccurateJumpPoint = JumpPoint;
			MinDistanceToPlayer = FVector::Distance(JumpPoint->GetActorLocation(), CurrentTargetLocation);
		}
	}
}
