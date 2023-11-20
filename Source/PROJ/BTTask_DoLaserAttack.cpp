// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoLaserAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PawnMovementComponent.h"

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
	return EBTNodeResult::InProgress; 
}

void UBTTask_DoLaserAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

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
	if (bDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Attacking!"));
		DrawDebugLine(GetWorld(), OwnerLocation, PlayerToAttackLocation, FColor::Orange, false, 0.3, 0, 2.f);
	}

	OwnerCharacter->Attack();
	OwnerCharacter->GetMovementComponent()->SetActive(true);
	
}
