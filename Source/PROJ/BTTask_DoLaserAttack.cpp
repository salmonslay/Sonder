// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoLaserAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"

UBTTask_DoLaserAttack::UBTTask_DoLaserAttack()
{
	NodeName = TEXT("DoLaserAttack");
}

void UBTTask_DoLaserAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_DoLaserAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = 1;

	if (OwnerComp.GetAIOwner() == nullptr) 	return EBTNodeResult::Failed; 

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return EBTNodeResult::Failed;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	UE_LOG(LogTemp, Error, TEXT("All pointers set correctly."));

	/*
	APROJCharacter* PlayerToAttack;
	UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("PlayerToAttack");

	if (PlayerObject == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	PlayerToAttack = Cast<APROJCharacter>(PlayerObject);
	
	if (PlayerToAttack)
	{
	*/
		OwnerCharacter->Attack();
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwnerCharacter);

		FHitResult Hit;
		bool bHit;

		if (bDebug)
		{
			bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerCharacter->GetActorForwardVector() * LaserDistance, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
		}
		else
		{
			bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerCharacter->GetActorForwardVector() * LaserDistance, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
		}

		if (bHit)
		{

			// cast to player 
			FDamageEvent DamageEvent;
			Hit.GetActor()->TakeDamage(OwnerCharacter->DamageToPlayer, DamageEvent, OwnerComp.GetAIOwner(), OwnerCharacter );
		}
		OwnerCharacter->bSetFocusToPlayer = true;
		return EBTNodeResult::Succeeded;
	//}
	//return EBTNodeResult::Failed;
}

void UBTTask_DoLaserAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
}
