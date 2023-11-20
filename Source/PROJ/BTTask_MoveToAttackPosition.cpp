// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToAttackPosition.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_MoveToAttackPosition::UBTTask_MoveToAttackPosition()
{
	NodeName = TEXT("MoveToAttackPosition");
}

void UBTTask_MoveToAttackPosition::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_MoveToAttackPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_MoveToAttackPosition::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	if (!OwnerCharacter->IsPathValid())
	{
		return;
	}

	// Check if the entity has reached the current waypoint.
	if (OwnerCharacter->CurrentPath.IsEmpty()) 
	{
		return;
	}
	int WaypointIndex = 0;
	FVector CurrentWaypoint = OwnerCharacter->CurrentPath[WaypointIndex];
	
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), CurrentWaypoint, 30.f, 40, FColor::Green, false, 3, 1, 0);
	}
	FVector DirectionToWaypoint = (CurrentWaypoint - OwnerLocation).GetSafeNormal();
	const float DistanceToWaypoint = DirectionToWaypoint.Length();
	

	// move to attack position
	CurrentTargetLocation = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector("StartAttackPosition");

	FVector DirectionToPlayer = CurrentTargetLocation - OwnerLocation;
	
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
	OwnerCharacter->SetActorRotation(DirectionToPlayer.Rotation());
}
