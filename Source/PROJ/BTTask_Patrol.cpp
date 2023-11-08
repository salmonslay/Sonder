// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"

#include "AIController.h"
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

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerCharacterLocation = OwnerCharacter->GetActorLocation();

	FVector ForwardPoint = OwnerCharacterLocation + (OwnerCharacter->GetActorForwardVector() * DistanceToCheck);
	FVector ForwardDownPoint = ForwardPoint + FVector(0, 0, -DownDistanceToCheck);


	OwnerComp.GetBlackboardComponent()->SetValueAsVector("ForwardPoint", ForwardPoint);

	if (bDebug)
	{
		DrawDebugLine(GetWorld(), OwnerCharacterLocation, ForwardPoint, FColor::Green, false, 2, 0, 1);
		DrawDebugLine(GetWorld(), OwnerCharacterLocation, ForwardDownPoint, FColor::Green, false, 2, 0, 1);
	}

	

	
	/*
	UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();

	FVector MovementVector = FVector::ZeroVector;


	MovementVector = OwnerCharacter->GetActorForwardVector() * OwnerCharacter->GetCharacterMovement()->GetMaxSpeed();


	// Set the initial desired movement direction (forward)
	FVector DesiredMovementDirection = FVector(1.0f, 0.0f, 0.0f);
	
	// find out which way is forward
	const FRotator Rotation = OwnerComp.GetAIOwner()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector 
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	OwnerCharacter->AddMovementInput(ForwardDirection, MovementVector.X);
	
*/
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwnerCharacter); // Ignore the actor itself if needed

	
	// Perform the line trace
	if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacterLocation, ForwardPoint, ECC_Visibility, CollisionParams)
		|| !GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCharacterLocation, ForwardDownPoint, ECC_Visibility, CollisionParams))
	{
		TurnAround();
	}
	
}

void UBTTask_Patrol::TurnAround()
{
	
}
