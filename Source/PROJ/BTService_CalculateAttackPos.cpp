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

void UBTService_CalculateAttackPos::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	const APROJCharacter* PlayerToAttack = Cast<APROJCharacter>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("CurrentTargetPlayer"));

	if (PlayerToAttack == nullptr)
	{
		return;
	}
	
	PlayerToAttackPosition = PlayerToAttack->GetActorLocation();

	if (!IsStartAttackPositionValid(OwnerComp.GetBlackboardComponent()->GetValueAsVector("StartAttackPosition")))
	{
		// Generate a random height within the specified bounds around player's Z-coordinate
		const float AllowedAttackHeightDiff = FMath::Min(FMath::Abs(FMath::Tan(OwnerCharacter->MaxAngleToAttack) * FVector::Distance(OwnerLocation, PlayerToAttackPosition)), OwnerCharacter->MaxAttackHeightDifference);
		const float RandomHeight = FMath::FRandRange(PlayerToAttackPosition.Z - AllowedAttackHeightDiff, PlayerToAttackPosition.Z + AllowedAttackHeightDiff);
		//float RandomAngle = FMath::FRandRange(0.0f, 2 * PI);
        
		// Calculate rand new position
		const FVector NewPosition = FVector(PlayerToAttackPosition.X, OwnerLocation.Y , PlayerToAttackPosition.Z +RandomHeight);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", NewPosition);
		if (bDebug)
		{
			DrawDebugSphere(GetWorld(), NewPosition, 20.f, 30, FColor::Cyan, false, 0.2f, 0, 5);
		}
	}
}

bool UBTService_CalculateAttackPos::IsStartAttackPositionValid(const FVector& PosToCheck) const
{
	// Calculate AllowedAttackHeightDiff
	const float AllowedAttackHeightDiff = FMath::Min(FMath::Abs(FMath::Tan(OwnerCharacter->MaxAngleToAttack) * FVector::Distance(OwnerLocation, PlayerToAttackPosition)), OwnerCharacter->MaxAttackHeightDifference);

	// Check if GeneratedPosition is within the allowed attack height difference
	if (PosToCheck.Z < PlayerToAttackPosition.Z - AllowedAttackHeightDiff || PosToCheck.Z > PlayerToAttackPosition.Z + AllowedAttackHeightDiff) {
		return false; // GeneratedPosition is outside the allowed height difference
	}
	return true;
}

