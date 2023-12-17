// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetValidPath.h"

#include "AIController.h"
#include "PROJCharacter.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Height Difference is valid, not jumping and is at same height ish"));
	}
	
	if (!OwnerCharacter->HasNavigationToTarget(CurrentTarget))
	{
		SetPathIsInvalid(OwnerComp);
		return; 
	}
	
	// Path valid 
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, true);
}

bool UBTService_SetValidPath::HasLineOfSightToPlayer(AShadowCharacter* Owner, APROJCharacter* CurrentPlayerTarget) const
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);
	ActorsToIgnore.Add(CurrentPlayerTarget);
		
	if (bDebug)
	{
		return !UKismetSystemLibrary::LineTraceSingleForObjects(this, Owner->GetActorLocation(), CurrentPlayerTarget->GetActorLocation(), LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FColor::Red, FColor::Blue, 2.f);
	}
	return !UKismetSystemLibrary::LineTraceSingleForObjects(this, Owner->GetActorLocation(), CurrentPlayerTarget->GetActorLocation(), LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

}


void UBTService_SetValidPath::SetPathIsInvalid(UBehaviorTreeComponent& OwnerComp) const
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, false); 
	OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.SelectedKeyName);
}
