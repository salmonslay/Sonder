// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_AlternateTargetPlayer.h"

#include "AIController.h"
#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "PROJCharacter.h"
#include "PROJGameMode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_AlternateTargetPlayer::UBTService_AlternateTargetPlayer()
{
	NodeName = TEXT("AlternateTargetPlayer");
}

void UBTService_AlternateTargetPlayer::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_AlternateTargetPlayer::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_AlternateTargetPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	/*
	if (OwnerCharacter == nullptr) return;

	OwnerController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (OwnerController == nullptr) return;
	*/

	APROJGameMode* CurrentGameMode = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(this));
	if (CurrentGameMode == nullptr) return;

	Player1 = CurrentGameMode->GetActivePlayer(0);
	Player2 = CurrentGameMode->GetActivePlayer(1);
	if (Player1 == nullptr)
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player1 is nullptr"));
		}
		return;
	}

	if (Player2 == nullptr)
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player2 is nullptr"));
		}
		return;
	}


	FVector CurrentTargetLocation;
	// choose player that is closest
	if (FVector::Distance(OwnerCharacter->GetActorLocation(), Player1->GetActorLocation()) >= FVector::Distance(OwnerCharacter->GetActorLocation(), Player2->GetActorLocation()))
	{
		CurrentTargetLocation = Player2->GetActorLocation();
		OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentTargetPlayer", Player2);

	}
	else
	{
		CurrentTargetLocation = Player1->GetActorLocation();
		OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentTargetPlayer", Player1);
	}
	if (bDebug)
	{
		DrawDebugSphere(OwnerCharacter->GetWorld(), CurrentTargetLocation, 30.f, 30, FColor::Red, false, 0.3f );
	}
}
