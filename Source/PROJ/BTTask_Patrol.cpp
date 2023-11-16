// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"

#include "AIController.h"
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	OwnerController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (OwnerController == nullptr) return;

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerCharacterLocation = OwnerCharacter->GetActorLocation();

	//OwnerCharacter->SetActorRotation(EnemyRotation);

	if (Player1 == nullptr || Player2 == nullptr)
	{
		Player1 = OwnerController->GetPlayerFromController(0);
		Player2 = OwnerController->GetPlayerFromController(1);
	}

	

	FVector ForwardPoint = OwnerCharacterLocation + (OwnerCharacter->GetActorForwardVector() * DistanceToCheck);
	FVector ForwardDownPoint = OwnerCharacterLocation + (OwnerCharacter->GetActorForwardVector() * DownForwardDistanceToCheck) + FVector(0, 0, -DownDistanceToCheck);


	//OwnerComp.GetBlackboardComponent()->SetValueAsVector("ForwardPoint", ForwardPoint);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), OwnerCharacterLocation, 30, 30, FColor::Green, false, 0.1, 0, 1);
		DrawDebugLine(GetWorld(), OwnerCharacterLocation, ForwardPoint, FColor::Green, false, 0.1, 0, 1);
		DrawDebugLine(GetWorld(), OwnerCharacterLocation, ForwardDownPoint, FColor::Green, false, 0.1, 0, 1);
	}
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerCharacter);
	CollisionParams.AddIgnoredActor(ECC_Pawn);

	
	
	// Perform the line trace
	if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacterLocation, ForwardPoint, ECC_Visibility, CollisionParams)
		||!(GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacterLocation, ForwardDownPoint, ECC_Visibility, CollisionParams)))
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Turn around"));
		}
		//FRotator NewRotation = FRotator(0.0f, OwnerCharacter->GetActorRotation().Yaw + 180.0f, 0.0f);
		EnemyRotation = FRotator(0.0f, OwnerCharacter->GetActorRotation().Yaw + 180.0f, 0.0f);
        OwnerCharacter->SetActorRotation(EnemyRotation);
	}
	OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorForwardVector(), PatrolSpeed);
}

