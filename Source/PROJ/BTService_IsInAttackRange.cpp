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

void UBTService_IsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	const APROJCharacter* PlayerToAttack = Cast<APROJCharacter>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("PlayerToAttack"));

	if (PlayerToAttack == nullptr)
	{
		return;
	}
	
	PlayerToAttackLocation = PlayerToAttack->GetActorLocation();
	
	const float HeightDiff = FMath::Abs(OwnerLocation.Z - PlayerToAttackLocation.Z);
	FVector VectorToPlayer = OwnerLocation - PlayerToAttackLocation;
	VectorToPlayer.Normalize();
	const float CurrentAngleToTarget = FMath::Acos(FMath::Abs(FVector::DotProduct(VectorToPlayer, FVector(0, 1, 0))));
	if (HeightDiff > OwnerCharacter->MaxAttackHeightDifference || CurrentAngleToTarget > OwnerCharacter->MaxAngleToAttack)
	{
		if((OwnerLocation - PlayerToAttackLocation).Length() > 50)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", false);
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", true);
		}
	}
	else
	{
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwnerCharacter);

		FHitResult Hit;
		bool bHit;

		
		if (bDebug)
		{
			//bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation + OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
			bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation + OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, 10, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration);
		}
		else
		{
			//bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation +OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
			bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation + OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, 10, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration);
		}

		if (bHit)
		{
			const APROJCharacter* PlayerHit = Cast<APROJCharacter>(Hit.GetActor());
			if (PlayerHit)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", true);
				OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", OwnerLocation);
			}
			else 
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", false);
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", true);
			OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", OwnerLocation);
		}
	}
}
