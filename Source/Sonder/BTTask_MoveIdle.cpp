// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveIdle.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_MoveIdle::UBTTask_MoveIdle()
{
	NodeName = TEXT("MoveIdle"); 
}

EBTNodeResult::Type UBTTask_MoveIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	const ACharacter* Owner = OwnerComp.GetAIOwner()->GetCharacter(); 

	Owner->GetCharacterMovement()->MaxWalkSpeed = IdleMoveSpeed;

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyMovingIdle.SelectedKeyName, true);

	StartLoc = Owner->GetActorLocation(); 

	SetTargetLocInit(OwnerComp);

	bNotifyTick = 1; 

	return Result; 
}

void UBTTask_MoveIdle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	const FVector TargetLoc = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BlackboardKey.SelectedKeyName);

	// Reached its target, "reverse" 
	if(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation().Equals(TargetLoc, 50.f))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, StartLoc); 
		StartLoc = TargetLoc; 
	}
}

void UBTTask_MoveIdle::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (!OwnerComp.GetAIOwner()) return;
	if (!OwnerComp.GetAIOwner()->GetCharacter()) return;
	if (!OwnerComp.GetAIOwner()->GetCharacter()->GetCharacterMovement()) return;
	
	OwnerComp.GetAIOwner()->GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = DefaultMoveSpeed;

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyMovingIdle.SelectedKeyName, false); 
	OwnerComp.GetBlackboardComponent()->ClearValue(BBKeyMovingIdle.SelectedKeyName); 
}

void UBTTask_MoveIdle::SetTargetLocInit(UBehaviorTreeComponent& OwnerComp) const
{
	const auto Owner = OwnerComp.GetAIOwner()->GetCharacter(); 
	const FVector CurrentPos = Owner->GetActorLocation();

	// Move right if currently not moving right 
	const bool bMoveRight = FVector::DotProduct(Owner->GetActorForwardVector(), FVector::RightVector) <= 0; 

	const FVector CurrentTarget = CurrentPos + FVector::RightVector * (bMoveRight ? MaxMoveDistance : -MaxMoveDistance); 

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, CurrentTarget);
}
