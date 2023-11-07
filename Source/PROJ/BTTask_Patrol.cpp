// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"

#include "AIController.h"
#include "EnemyCharacter.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");
}

void UBTTask_Patrol::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// code below makes it so TickTask is called 
	bNotifyTick = 1;
	return EBTNodeResult::InProgress; 
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	FVector ForwardPoint = OwnerCharacter->GetActorLocation() + (OwnerCharacter->GetActorForwardVector() * DistanceToCheck);
	FVector ForwardDownPoint = OwnerCharacter->GetActorLocation() + (OwnerCharacter->GetActorForwardVector() * DistanceToCheck * DownDistanceToCheck);
	
	if (bDebug)
	{
		//DrawDebugSphere(GetWorld(), OwnerCharacter->GetActorLocation() , 30.f, 24, FColor::Purple, false, .2f);
		DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation() , ForwardPoint, FColor::Green, false, 2, 0, 1);
		DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation() , ForwardDownPoint, FColor::Green, false, 2, 0, 1);

	}

	/*

	// Define the end location for the line trace, assuming you want to trace downwards
	FVector EndLocation = StartLocation - FVector(0, 0, 1000); // Adjust the Z value as needed

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore the actor itself if needed

	// Perform the line trace
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams))
	{
		// The line trace hit something
		AActor* HitActor = HitResult.GetActor();
		FVector ImpactPoint = HitResult.ImpactPoint;

		// You can perform further actions based on the hit result
	}
	*/
}
