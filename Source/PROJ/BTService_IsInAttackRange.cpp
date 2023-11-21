// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsInAttackRange.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

class APROJCharacter;

UBTService_IsInAttackRange::UBTService_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");
}

void UBTService_IsInAttackRange::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_IsInAttackRange::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_IsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	APROJCharacter* PlayerToAttack;
	UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("PlayerToAttack");

	if (PlayerObject == nullptr)
	{
		return;
	}
	PlayerToAttack = Cast<APROJCharacter>(PlayerObject);
	if (PlayerToAttack)
	{
		PlayerToAttackLocation = PlayerToAttack->GetActorLocation();
	}
	
	const float HeightDiff = FMath::Abs(OwnerLocation.Z - PlayerToAttackLocation.Z);
	if (HeightDiff > OwnerCharacter->MaxAttackHeightDifference)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", false);
	}
	else
	{
		OwnerCharacter->Attack();
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwnerCharacter);

		FHitResult Hit;
		bool bHit;

		if (bDebug)
		{
			bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation + OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
		}
		else
		{
			bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation +OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
		}

		if (bHit)
		{
			APROJCharacter* PlayerHit = Cast<APROJCharacter>(Hit.GetActor());
			if (!PlayerHit)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", false);
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", true);
			OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", OwnerLocation);
		}
		if (bDebug)
		{
			DrawDebugLine(GetWorld(), OwnerLocation, PlayerToAttackLocation, FColor::Cyan, false, 0.2f, 0, 5.f);
		}
	}
}
