// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetValidPath.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SetValidPath::UBTService_SetValidPath()
{
	NodeName = TEXT("SetValidPath"); 
}

void UBTService_SetValidPath::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const FVector CurrentTarget = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKeyCurrentMoveTarget.SelectedKeyName);

	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();

	if(FMath::Abs(Owner->GetActorLocation().Z - CurrentTarget.Z) > HeightDifferenceToMarkInvalid)
	{
		SetPathIsInvalid(OwnerComp);
		return; 
	}

	const auto Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, Owner->GetActorLocation(), CurrentTarget, Owner);
	
	if(Path->IsPartial() || Path->IsUnreachable())
	{
		SetPathIsInvalid(OwnerComp); 
		return; 
	} 

	// Path valid 
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, true); 
}

void UBTService_SetValidPath::SetPathIsInvalid(UBehaviorTreeComponent& OwnerComp) const
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, false); 
	OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.SelectedKeyName);
}
