// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetDashThroughRobot.h"

#include "AIController.h"
#include "ShadowRobotCharacter.h"
#include "StaticsHelper.h"
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
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyDashThroughRobot.SelectedKeyName, false);
		OwnerComp.GetBlackboardComponent()->ClearValue(BBKeyDashThroughRobot.SelectedKeyName); 
		return; 
	}

	const auto OwnerPawn = OwnerComp.GetAIOwner()->GetPawn(); 

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKeyDashThroughRobot.SelectedKeyName, ShouldDashThroughRobot(OwnerPawn));
}

bool UBTService_SetDashThroughRobot::IsAttacking(UBehaviorTreeComponent& OwnerComp) const
{
	// Doing basic attack, return true 
	if(OwnerComp.GetBlackboardComponent()->GetValueAsBool(BBKeyDoBasicAttack.SelectedKeyName))
		return true;

	// Not basic attack, return if special attack can be performed 
	return OwnerComp.GetBlackboardComponent()->GetValueAsBool(BBKeyDoSpecialAttack.SelectedKeyName); 
}

bool UBTService_SetDashThroughRobot::ShouldDashThroughRobot(const APawn* Owner) const
{
	// Get all ShadowRobots in dash distance, i.e potential targets 
	const TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OverlappingActors;
	
	UKismetSystemLibrary::SphereOverlapActors(this, Owner->GetActorLocation(), DistanceToRobotToDash,
	ShadowObjectType, AShadowRobotCharacter::StaticClass(), ActorsToIgnore, OverlappingActors);

	// Check if any Robot is in movement direction, if it is then we can buff a Robot 
	for(const auto Robot : OverlappingActors)
		if(UStaticsHelper::ActorIsInFront(Owner, Robot))
			return true; 

	return false; 
}
