// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_ChooseAttack.h"

#include "AIController.h"
#include "BasicAttackComponent.h"
#include "RobotBaseState.h"
#include "ShadowSoulCharacter.h"
#include "SoulBaseStateNew.h"
#include "SoulDashingState.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_ChooseAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	const auto OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();

	if(!bHasSetUpComps)
	{
		AttackComp = OwnerPawn->FindComponentByClass<UBasicAttackComponent>();
		SoulState = OwnerPawn->FindComponentByClass<USoulBaseStateNew>();
		RobotState = OwnerPawn->FindComponentByClass<URobotBaseState>();
		bHasSetUpComps = true; 
	}

	// Do nothing if AI is charging or using its special attack 
	if(IsUsingSpecialAttack(OwnerComp)) 
		return; 
	
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

	// In range for special attack, check if in range for basic attack, if we are then determine attack by weights  
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

bool UBTService_ChooseAttack::IsUsingSpecialAttack(UBehaviorTreeComponent& OwnerComp) const
{
	// Charging 
	if(OwnerComp.GetBlackboardComponent()->GetValueAsBool(BBKeyChargingSpecialAttack.SelectedKeyName))
		return true;

	// Using (currently only applies to Dash since Pulse is instant, hook shot will need to be checked if implemented)
	if(const auto Soul = Cast<AShadowSoulCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(Soul->GetCurrentState() == Soul->DashState) 
			return true; 
	}

	return false; // Not charging or using 
}

bool UBTService_ChooseAttack::IsSpecialAttackOnCooldown(const APawn* Owner) const
{
	if(RobotState)
		return RobotState->IsPulseCoolDownActive(); 
		
	if (SoulState)
		return SoulState->bDashCoolDownActive;

	return true; 
}

bool UBTService_ChooseAttack::CanBasicAttack(const APawn* Owner) const
{
	return AttackComp && AttackComp->CanAttack(); 
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
