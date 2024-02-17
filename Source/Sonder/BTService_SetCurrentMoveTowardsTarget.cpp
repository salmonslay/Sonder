// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetCurrentMoveTowardsTarget.h"

#include "AIController.h"
#include "EnemyJumpPoint.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

namespace JumpPointMovement
{
	bool bDebugDrawJumpPointTarget = false;
	FAutoConsoleVariableRef CVarDebugDrawJumpPointTarget(TEXT("sonder.DebugDrawJumpPointTarget"), bDebugDrawJumpPointTarget, TEXT("Show current jump point target"), ECVF_Cheat);
}

UBTService_SetCurrentMoveTowardsTarget::UBTService_SetCurrentMoveTowardsTarget()
{
	NodeName = TEXT("SetCurrentMoveTowardsTarget");
}

void UBTService_SetCurrentMoveTowardsTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!IsValid(&OwnerComp) || OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}

	OwnerCharacter = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter.Get() == nullptr)
	{
		return;
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
		return;
	}

	CurrentTargetLocation = OwnerCharacter->CurrentTargetLocation;

	if (OwnerCharacter->IsLeveledWithLocation(CurrentTargetLocation))
	{
		BlackboardComponent->SetValueAsBool("bIsNearlySameHeightAsPlayer", true);
	}
	else
	{
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
#if !UE_BUILD_SHIPPING
		if (JumpPointMovement::bDebugDrawJumpPointTarget && ClosestMoveLoc != FVector::ZeroVector)
		{
			DrawDebugSphere(GetWorld(), ClosestMoveLoc, 30.f, 5, FColor::Orange, false, 0.5f);
		}
#endif //!UE_BUILD_SHIPPING
		
		if (ClosestMoveLoc == FVector::ZeroVector)
		{
			ClosestMoveLoc = GetClosestJumpPointTo(CurrentTargetLocation);
#if !UE_BUILD_SHIPPING
			if (JumpPointMovement::bDebugDrawJumpPointTarget && ClosestMoveLoc != FVector::ZeroVector)
			{
				DrawDebugSphere(GetWorld(), ClosestMoveLoc, 30.f, 5, FColor::Turquoise, false, 0.5f);
			}
#endif //!UE_BUILD_SHIPPING
		}
		BlackboardComponent->SetValueAsVector(BBKeyClosestMoveTarget.SelectedKeyName,  FVector(OwnerLocation.X, ClosestMoveLoc.Y, OwnerLocation.Z ) );
		
		BlackboardComponent->SetValueAsBool("bIsNearlySameHeightAsPlayer", false);
		BlackboardComponent->ClearValue("bIsNearlySameHeightAsPlayer");
	}
}

FVector UBTService_SetCurrentMoveTowardsTarget::GetClosestReachableJumpPointLocation()
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

FVector UBTService_SetCurrentMoveTowardsTarget::GetClosestJumpPointTo(const FVector& Loc)
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

TWeakObjectPtr<AEnemyJumpPoint> UBTService_SetCurrentMoveTowardsTarget::GetJumpPointFromLocation(const FVector& Loc)
{
	TWeakObjectPtr<AEnemyJumpPoint> Temp;
	for (const auto JumpPoint : JumpPoints)
	{
		if (JumpPoint->GetActorLocation() == Loc)
		{
			Temp = JumpPoint;
		}
	}
	return Temp;
}
