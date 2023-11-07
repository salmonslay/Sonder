// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackPlayer.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = TEXT("AttackPlayer");
}

void UBTTask_AttackPlayer::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// code below makes it so TickTask is called 
	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_AttackPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;


	APROJCharacter* PlayerToAttack = nullptr;
	UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("PlayerToAttack");
	if (PlayerObject)
	{
		PlayerToAttack = Cast<APROJCharacter>(PlayerObject);
	}
	if (PlayerToAttack && IsValid(PlayerToAttack))
	{
		OnAttackPlayer();
		OwnerCharacter->OnAttackEvent();
	
		UGameplayStatics::ApplyDamage(PlayerToAttack, OwnerCharacter->DamageToPlayer, OwnerCharacter->GetController(), OwnerCharacter, nullptr);
		if (bDebug)
		{
			
			DrawDebugSphere(GetWorld(), PlayerToAttack->GetActorLocation(), 30.f, 24, FColor::Purple, false, .2f);
			UE_LOG(LogTemp, Warning, TEXT("Attacked player and damage %f"), OwnerCharacter->DamageToPlayer);
		}
	}

	OwnerComp.GetBlackboardComponent()->ClearValue("PlayerToAttack");
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("bFoundPlayerWithinAttackRadius", false);
}
