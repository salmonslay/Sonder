// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoStun.h"

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

	bNotifyTick = 1;

	return EBTNodeResult::InProgress; 
}

void UBTTask_DoStun::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


/*
	
	if (OwnerComp.GetAIOwner() == nullptr) 	return; 

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;
	Timer += DeltaSeconds;
	if (Timer > OwnerComp.GetAIOwner->Get)
	*/
}


// Stunned task: in progress, in tick count up timer, when timer = stunned duration = taskstatus completed

// Stunned task activated - reset timer. Tick the timer in tick, timer can max be stunned duration.