// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetValidPath.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "ShadowCharacter.h"
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

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;
	
	OwnerLocation = OwnerCharacter->GetActorLocation();

	if(FMath::Abs(OwnerLocation.Z - CurrentTarget.Z) > HeightDifferenceToMarkInvalid)
	{
		SetPathIsInvalid(OwnerComp);
		return; 
	}
	if (!OwnerCharacter->HasNavigationTo(CurrentTarget))
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
