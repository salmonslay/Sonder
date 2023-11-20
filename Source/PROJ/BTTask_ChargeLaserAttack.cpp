// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChargeLaserAttack.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_ChargeLaserAttack::UBTTask_ChargeLaserAttack()
{
	NodeName = TEXT("ChargeLaserAttack");
}

void UBTTask_ChargeLaserAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_ChargeLaserAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_ChargeLaserAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	if (bDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Charging attack!"));
		//DrawDebugSphere(GetWorld(), OwnerLocation, 40, 40, FColor::Orange, false, 0.3, 0, 2.f);
	}

	OwnerCharacter->ChargeAttack();
	OwnerCharacter->GetCharacterMovement()->StopActiveMovement();
	OwnerCharacter->GetMovementComponent()->SetActive(false);
	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsFloat("ChargingTime", OwnerCharacter->ChargeAttackDuration);
}
