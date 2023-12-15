// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoStun.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_DoStun::UBTTask_DoStun()
{
	NodeName = TEXT("DoStun");
}

void UBTTask_DoStun::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);

	Timer = 0.f;
}

EBTNodeResult::Type UBTTask_DoStun::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	if (OwnerComp.GetAIOwner() == nullptr) 	return EBTNodeResult::Failed; 

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return EBTNodeResult::Failed;

	StunDuration = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsFloat("StunnedDuration");

	if (Timer < StunDuration)
	{
		bNotifyTick = 1;
		if(const auto FlyingEnemy = Cast<AFlyingEnemyCharacter>(OwnerCharacter))
			FlyingEnemy->bSetFocusToPlayer = false;

		if(const auto ShadowChar = Cast<AShadowCharacter>(OwnerCharacter))
			ShadowChar->ServerRPC_ToggleChargeEffect(false); 

		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Succeeded;
}

void UBTTask_DoStun::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	Timer += DeltaSeconds;

	StunDuration = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsFloat("StunnedDuration");
	if (Timer > StunDuration)
	{
		if(const auto FlyingEnemy = Cast<AFlyingEnemyCharacter>(OwnerCharacter))
			FlyingEnemy->bSetFocusToPlayer = true;
		
		OwnerCharacter->bIsStunned = false;
		//OwnerCharacter->Idle();
		OwnerCharacter->GetCharacterMovement()->CurrentRootMotion.Clear();
		OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		Timer = 0.f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
