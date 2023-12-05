// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_ChooseAttack.h"

#include "AIController.h"
#include "EnemyBasicAttack.h"
#include "PlayerBasicAttack.h"
#include "RobotBaseState.h"
#include "ShadowRobotCharacter.h"
#include "ShadowSoulCharacter.h"
#include "SoulBaseStateNew.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_ChooseAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto OwnerPawn = OwnerComp.GetAIOwner()->GetPawn(); 

	const float DistanceToTarget = FVector::Dist(OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKeyCurrentTarget.SelectedKeyName), OwnerPawn->GetActorLocation());

	// Too far away to attack, set cant attack to both attacks 
	if(DistanceToTarget > DistanceToSpecialAttack)
	{
		SetBasicAttackBBKey(OwnerComp, false); 
		SetSpecialAttackBBKey(OwnerComp, false); 
		return; 
	}
	
	// Special attack on cooldown, so cant perform it. Check if in range for basic attack and if it's on cooldown 
	if(IsSpecialAttackOnCooldown(OwnerPawn))
	{
		SetBasicAttackBBKey(OwnerComp, IsInRangeForBasicAttack(DistanceToTarget));
		SetSpecialAttackBBKey(OwnerComp, false); 
		return; 
	}

	// In range for special attack, check if in range for basic attack 
	if(IsInRangeForBasicAttack(DistanceToTarget))
	{
		if(CanBasicAttack(OwnerPawn) && FMath::RandRange(0.f, 1.f) < ProbabilityToBasicAttack)
		{
			SetBasicAttackBBKey(OwnerComp, true); 
			SetSpecialAttackBBKey(OwnerComp, false); 
		} else
		{
			SetBasicAttackBBKey(OwnerComp, false); 
			SetSpecialAttackBBKey(OwnerComp, true); 
		}
		return; 
	} 

	// Too far away for basic but close enough for special
	SetBasicAttackBBKey(OwnerComp, false); 
	SetSpecialAttackBBKey(OwnerComp, FMath::RandRange(0.f, 1.f) < ProbabilityToSpecialAttack);
	
}

bool UBTService_ChooseAttack::IsSpecialAttackOnCooldown(APawn* Owner) const
{
	if(const auto Robot = Cast<AShadowRobotCharacter>(Owner))
		return Robot->FindComponentByClass<URobotBaseState>()->IsPulseCoolDownActive(); 
		
	if (const auto Soul = Cast<AShadowSoulCharacter>(Owner))
		return Soul->FindComponentByClass<USoulBaseStateNew>()->bDashCoolDownActive;

	return true; 
}

bool UBTService_ChooseAttack::CanBasicAttack(const APawn* Owner) const
{
	if(const auto BasicAttack = Owner->FindComponentByClass<UEnemyBasicAttack>())
		return BasicAttack->CanAttack();

	return false; 
}

bool UBTService_ChooseAttack::IsInRangeForBasicAttack(const float DistanceToTarget) const
{
	return DistanceToTarget < DistanceToBasicAttack; 
}

void UBTService_ChooseAttack::SetBasicAttackBBKey(UBehaviorTreeComponent& OwnerComp, const bool bCanDoAttack) const
{
	if(bCanDoAttack) 
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyDoBasicAttack.SelectedKeyName, true);
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyDoBasicAttack.SelectedKeyName, false);
		OwnerComp.GetBlackboardComponent()->ClearValue(BBKeyDoBasicAttack.SelectedKeyName);
	}
}

void UBTService_ChooseAttack::SetSpecialAttackBBKey(UBehaviorTreeComponent& OwnerComp, const bool bCanDoAttack) const
{
	if(bCanDoAttack) 
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyDoSpecialAttack.SelectedKeyName, true);
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyDoSpecialAttack.SelectedKeyName, false);
		OwnerComp.GetBlackboardComponent()->ClearValue(BBKeyDoSpecialAttack.SelectedKeyName);
	}
}
