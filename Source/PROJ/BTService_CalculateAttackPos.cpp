// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CalculateAttackPos.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_CalculateAttackPos::UBTService_CalculateAttackPos()
{
	NodeName = TEXT("CalculateAttackPosition");
}

void UBTService_CalculateAttackPos::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_CalculateAttackPos::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_CalculateAttackPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		PlayerToAttackPosition = PlayerToAttack->GetActorLocation();
	}
	// Generate a random height within the specified bounds around player's Z-coordinate
	float RandomHeight = FMath::FRandRange(PlayerToAttackPosition.Z - OwnerCharacter->MaxAttackHeightDifference, PlayerToAttackPosition.Z + OwnerCharacter->MaxAttackHeightDifference);
	//float RandomAngle = FMath::FRandRange(0.0f, 2 * PI);
        
	// Calculate rand new position
	FVector NewPosition = FVector(PlayerToAttackPosition.X, PlayerToAttackPosition.Y , OwnerLocation.Z +RandomHeight);
	OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", NewPosition);
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), NewPosition, 20.f, 30, FColor::Cyan, false, 0.2f, 0, 5);
	}
	
}

