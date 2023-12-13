// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetCurrentTarget.h"

#include "EnemyAIController.h"
#include "PROJCharacter.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UBTService_SetCurrentTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, GetTargetLocation(OwnerComp.GetAIOwner())); 
}

FVector UBTService_SetCurrentTarget::GetTargetLocation(AAIController* BaseAIController)
{
	OwnerCharacter = Cast<AShadowCharacter>(BaseAIController->GetCharacter());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not cast AI character in Service Set Current target"))
		return FVector::Zero(); 
	}
	
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

		// Respawning/in arena jail 
		if(Player1->bIsSafe)
		{
			OwnerCharacter->CurrentTargetLocation = Player2->GetActorLocation();
			return OwnerCharacter->CurrentTargetLocation; 
		}

		const bool bLOSToP1 = HasLineOfSightToPlayer(OwnerCharacter, Player1);
		const bool bLOSToP2 = HasLineOfSightToPlayer(OwnerCharacter, Player2); 

		// P1 closer and has LOS or no LOS to p2, then set p1 as target 
		// P2 closer but no LOS to P2 and LOS to p1, then also set p1 as target 
		if((DistToPlayerOne < DistToPlayerTwo && (bLOSToP1 || !bLOSToP2)) || DistToPlayerTwo < DistToPlayerOne && (!bLOSToP2 && bLOSToP1))
		{
			OwnerCharacter->CurrentTargetLocation = Player1->GetActorLocation();
			return OwnerCharacter->CurrentTargetLocation;
		}

		// Otherwise, p2 is target 
		OwnerCharacter->CurrentTargetLocation = Player2->GetActorLocation();
		return OwnerCharacter->CurrentTargetLocation; 
	}

	UE_LOG(LogTemp, Error, TEXT("Could not cast AI controller in Service Set Current target"))
	return FVector::Zero(); 
}

bool UBTService_SetCurrentTarget::HasLineOfSightToPlayer(AShadowCharacter* Owner, class APROJCharacter* PlayerTarget) const
{
	FHitResult HitResult;
	const TArray<AActor*> ActorsToIgnore { Owner, PlayerTarget }; 

	return !UKismetSystemLibrary::LineTraceSingleForObjects(this, Owner->GetActorLocation() + FVector::UpVector * 20.f, PlayerTarget->GetActorLocation(), LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
}
