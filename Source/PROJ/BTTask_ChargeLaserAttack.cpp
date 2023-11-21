// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChargeLaserAttack.h"

#include "AIController.h"
#include "EditorReimportHandler.h"
#include "FlyingEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/LowLevelTestAdapter.h"

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

	if (OwnerComp.GetAIOwner() == nullptr) return EBTNodeResult::Failed;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return EBTNodeResult::Failed;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsFloat("ChargingTime", OwnerCharacter->ChargeAttackDuration);

	OwnerCharacter->ChargeAttack();
	OwnerCharacter->bSetFocusToPlayer = false;
	if (bDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Charging attack!"));
		//DrawDebugSphere(GetWorld(), OwnerLocation, 40, 40, FColor::Orange, false, 0.3, 0, 2.f);
	}
	//OwnerCharacter->GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
	OwnerCharacter->GetCharacterMovement()->CurrentRootMotion.Clear();
	OwnerCharacter->GetCharacterMovement()->Velocity= FVector::ZeroVector;

	//OwnerCharacter->GetCharacterMovement()->SetPlaneConstraintEnabled(true);
		//GetWorld()->GetTimerManager().SetTimer(EnableMovementTimerHandle, this, &UBTTask_ChargeLaserAttack::EnableMovement, OwnerCharacter->ChargeAttackDuration, false, -1.f);
	return EBTNodeResult::Succeeded;
}

void UBTTask_ChargeLaserAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	
}

void UBTTask_ChargeLaserAttack::EnableMovement()
{
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	bIsCharging = false;
}
