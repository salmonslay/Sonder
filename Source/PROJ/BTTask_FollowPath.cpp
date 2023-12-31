// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FollowPath.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "FlyingEnemyCharacter.h"
#include "Grid.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

UBTTask_FollowPath::UBTTask_FollowPath()
{
	NodeName = TEXT("FollowPath");
}

EBTNodeResult::Type UBTTask_FollowPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// code below makes it so TickTask is called 
	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_FollowPath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerCharacter->Idle();
	
	OwnerLocation = OwnerCharacter->GetActorLocation();

	// Check if the entity has reached the current waypoint.
	if (OwnerCharacter->CurrentPath.IsEmpty()) 
	{
		if(const AActor* PlayerOne = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("PlayerOne")))
		{
			OwnerCharacter->CurrentPath = OwnerCharacter->GetGridPointer()->RequestPath(OwnerLocation, PlayerOne->GetActorLocation(), false);
			if(OwnerCharacter->CurrentPath.IsEmpty()) { return; }
		}
		else
		{
			return;
		}
	}
	int WaypointIndex = 0;
	FVector CurrentWaypoint = OwnerCharacter->CurrentPath[WaypointIndex];
	
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), CurrentWaypoint, 30.f, 40, FColor::Green, false, 3, 1, 0);
	}
	FVector DirectionToWaypoint = (CurrentWaypoint - OwnerLocation).GetSafeNormal();
	const float DistanceToWaypoint = DirectionToWaypoint.Length();
	
	APROJCharacter* PlayerToAttack = Cast<APROJCharacter>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("CurrentTargetPlayer"));

	if (PlayerToAttack == nullptr)
	{
		return;
	}
	
	OwnerCharacter->CurrentTargetPlayer = PlayerToAttack;
	
	// Entity has reached the current waypoint
	if (DistanceToWaypoint > 1.f)
	{
		WaypointIndex++;
	}
		
	// Check if there are more waypoints left
	if (OwnerCharacter->CurrentPath.IsEmpty())
	{
		return;
	}
	
	if (WaypointIndex < OwnerCharacter->CurrentPath.Num())
	{
		CurrentWaypoint = OwnerCharacter->CurrentPath[WaypointIndex];
	}
	
	DirectionToWaypoint = (CurrentWaypoint - OwnerLocation).GetSafeNormal();

	if (bDebug)
	{
		DrawDebugLine(GetWorld(), OwnerLocation, (OwnerLocation * DirectionToWaypoint) * 5.f, FColor::Red, false, 0.2, 0, 5.f);
	}

	OwnerCharacter->GetMovementComponent()->SetActive(true);
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	OwnerCharacter->GetCharacterMovement()->AddInputVector(DirectionToWaypoint * PathFollowingSpeed);
}
