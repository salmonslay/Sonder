// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetCurrentTarget.h"

#include "EnemyAIController.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_SetCurrentTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, GetTargetLocation(OwnerComp.GetAIOwner())); 
}

FVector UBTService_SetCurrentTarget::GetTargetLocation(AAIController* BaseAIController)
{
	if(const auto AIController = Cast<AEnemyAIController>(BaseAIController))
	{
		const auto Player1 = AIController->GetPlayerFromController(0);
		const auto Player2 = AIController->GetPlayerFromController(1);

		if(!Player1 || !Player2)
		{
			UE_LOG(LogTemp, Error, TEXT("No player 1 or 2 in Service Set Current Target"))
			return BaseAIController->GetPawn()->GetActorLocation();
		}

		const FVector CurrentLocation = BaseAIController->GetPawn()->GetActorLocation();

		const auto DistToPlayerOne = FVector::Dist(Player1->GetActorLocation(), CurrentLocation);
		const auto DistToPlayerTwo = FVector::Dist(Player2->GetActorLocation(), CurrentLocation);

		if(DistToPlayerOne < DistToPlayerTwo)
			return Player1->GetActorLocation();

		return Player2->GetActorLocation(); 
	}

	UE_LOG(LogTemp, Error, TEXT("Could not cast AI controller in Service Set Current target"))
	return FVector::Zero(); 
}
