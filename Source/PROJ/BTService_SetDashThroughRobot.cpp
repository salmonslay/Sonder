// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetDashThroughRobot.h"

#include "AIController.h"
#include "ShadowRobotCharacter.h"
#include "SoulDashingState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBTService_SetDashThroughRobot::UBTService_SetDashThroughRobot()
{
	NodeName = TEXT("SetDashThroughRobot"); 
}

void UBTService_SetDashThroughRobot::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if(IsAttacking(OwnerComp))
	{
		// Clear special attack value?
		return; 
	}

	const auto OwnerPawn = OwnerComp.GetAIOwner()->GetPawn(); 

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyDoSpecialAttack.SelectedKeyName, ShouldDashThroughRobot(OwnerPawn));
}

bool UBTService_SetDashThroughRobot::IsAttacking(UBehaviorTreeComponent& OwnerComp) const
{
	// We might want to check this differently, cooldowns etc. can make this work poorly 
	
	// Doing basic attack, return true 
	if(OwnerComp.GetBlackboardComponent()->GetValueAsBool(BBKeyDoBasicAttack.SelectedKeyName))
		return true;

	// Not basic attack, return if special attack can be performed 
	return OwnerComp.GetBlackboardComponent()->GetValueAsBool(BBKeyDoSpecialAttack.SelectedKeyName); 
}

bool UBTService_SetDashThroughRobot::ShouldDashThroughRobot(const APawn* Owner) const
{
	if(const auto DashState = Owner->FindComponentByClass<USoulDashingState>())
	{
		const float DashDistance = DashState->GetMaxDashDistance();

		// Get all ShadowRobots in dash distance, i.e potential targets 
		const TArray<AActor*> ActorsToIgnore;
		TArray<AActor*> OverlappingActors;
		
		UKismetSystemLibrary::SphereOverlapActors(this, Owner->GetActorLocation(), DashDistance,
			ShadowObjectType, AShadowRobotCharacter::StaticClass(), ActorsToIgnore, OverlappingActors);

		// Check if any Robot is in movement direction, if it is then we can buff a Robot 
		for(const auto Robot : OverlappingActors)
			if(IsRobotInMovementDirection(Robot, Owner))
				return true; 
	}

	return false; 
}

bool UBTService_SetDashThroughRobot::IsRobotInMovementDirection(const AActor* Robot, const APawn* Owner) const
{
	const FVector DirToRobot = Robot->GetActorLocation() - Owner->GetActorLocation();

	// Check if Robot is in front of Soul 
	return FVector::DotProduct(DirToRobot, Owner->GetActorForwardVector()) > 0; 
}
