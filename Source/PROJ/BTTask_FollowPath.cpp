// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FollowPath.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FollowPath::UBTTask_FollowPath()
{

	NodeName = TEXT("FollowPath");
}

void UBTTask_FollowPath::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
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


	APROJCharacter* PlayerToAttack;
	UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("CurrentTargetPlayer");

	if (PlayerObject == nullptr)
	{
		return;
	}
	PlayerToAttack = Cast<APROJCharacter>(PlayerObject);
	if (PlayerToAttack)
	{
		CurrentTargetLocation = PlayerToAttack->GetActorLocation();
	}

	FVector DirectionToPlayer = CurrentTargetLocation - OwnerLocation;
	
	// Entity has reached the current waypoint
	if (DistanceToWaypoint >= 1.f)
	{
		WaypointIndex++;
	}
		
	// Check if there are more waypoints left
	if (OwnerCharacter->CurrentPath.IsEmpty())
	{
		return;
	}
	
	if (OwnerCharacter->CurrentPath.Num() > WaypointIndex)
	{
		CurrentWaypoint = OwnerCharacter->CurrentPath[WaypointIndex];
	}
	
	//FRotator EnemyRotation = FRotator(DirectionToPlayer.Rotation(), 0.0f, 0.0f);
	//OwnerCharacter->SetActorRotation(EnemyRotation);
	DirectionToWaypoint = (CurrentWaypoint - OwnerLocation).GetSafeNormal();
	OwnerCharacter->AddMovementInput(DirectionToWaypoint, PathFollowingSpeed);
}
