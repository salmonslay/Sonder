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

		if(DistToPlayerOne < DistToPlayerTwo)
		{
			if(HasLineOfSightToPlayer(OwnerCharacter, Player1) || !HasLineOfSightToPlayer(OwnerCharacter, Player2))
			{
				OwnerCharacter->CurrentTargetLocation = Player1->GetActorLocation();
				return OwnerCharacter->CurrentTargetLocation;
			}
		}
		
		OwnerCharacter->CurrentTargetLocation = Player2->GetActorLocation();
		return OwnerCharacter->CurrentTargetLocation; 
	}

	UE_LOG(LogTemp, Error, TEXT("Could not cast AI controller in Service Set Current target"))
	return FVector::Zero(); 
}

bool UBTService_SetCurrentTarget::HasLineOfSightToPlayer(AShadowCharacter* Owner, class APROJCharacter* PlayerTarget)
{
	FHitResult HitResult;
	const TArray<AActor*> ActorsToIgnore { OwnerCharacter, PlayerTarget }; 

	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, OwnerCharacter->GetActorLocation() + FVector::UpVector * 20.f, PlayerTarget->GetActorLocation(), LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 30.f, 30, FColor::Red, false, 1.f);
		UE_LOG(LogTemp, Error, TEXT("Hit %s"), *HitResult.GetActor()->GetName())
	}
	return !bHit;
}

