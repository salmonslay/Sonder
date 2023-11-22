// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoStun.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
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

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return EBTNodeResult::Failed;

	StunDuration = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsFloat("StunnedDuration");

	if (Timer < StunDuration)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("NotStun"));	
		OwnerCharacter->bSetFocusToPlayer = true;
		OwnerCharacter->Idle();
		bNotifyTick = 1;
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UBTTask_DoStun::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	Timer += DeltaSeconds;

	StunDuration = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsFloat("StunnedDuration");
	if (Timer > StunDuration)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Stun"));	
		OwnerCharacter->Stun(StunDuration);
		OwnerCharacter->GetCharacterMovement()->CurrentRootMotion.Clear();
		OwnerCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		Timer = 0.f;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
