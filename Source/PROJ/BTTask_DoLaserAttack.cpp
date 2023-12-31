// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoLaserAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_DoLaserAttack::UBTTask_DoLaserAttack()
{
	NodeName = TEXT("DoLaserAttack");
}

EBTNodeResult::Type UBTTask_DoLaserAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = 1;

	if (OwnerComp.GetAIOwner() == nullptr) 	return EBTNodeResult::Failed; 

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return EBTNodeResult::Failed;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsFloat("PerformAttackTime", OwnerCharacter->PerformAttackDuration);

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
		if (PlayerHit)
		{
			const FDamageEvent DamageEvent;
			PlayerHit->TakeDamage(OwnerCharacter->DamageToPlayer, DamageEvent, OwnerComp.GetAIOwner(), OwnerCharacter);
		}
	}
	return EBTNodeResult::Succeeded;
	
}

