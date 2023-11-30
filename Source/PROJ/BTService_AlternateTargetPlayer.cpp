// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_AlternateTargetPlayer.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "PROJCharacter.h"
#include "SonderGameState.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_AlternateTargetPlayer::UBTService_AlternateTargetPlayer()
{
	NodeName = TEXT("AlternateTargetPlayer");
}

void UBTService_AlternateTargetPlayer::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);

	SGS = Cast<ASonderGameState>(GetWorld()->GetGameState());
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
	

	if (SGS == nullptr)
	{
		SGS = Cast<ASonderGameState>(GetWorld()->GetGameState());
		if (SGS == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid game state"));
			return;
		}
	}
	

	ServerPlayer = SGS->GetServerPlayer();
	ClientPlayer = SGS->GetClientPlayer();

	if (ServerPlayer == nullptr)
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player1 is nullptr"));
		}
		return;
	}
	
	if (ClientPlayer == nullptr)
	{
		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player2 is nullptr"));
		}
		return;
	}
	
	FVector CurrentTargetLocation;
	FVector OwnerLocation = OwnerCharacter->GetActorLocation();

	// check if both players are alive
	if(!ClientPlayer->bIsSafe && !ServerPlayer->bIsSafe && !OwnerComp.GetBlackboardComponent()->GetValueAsBool("bIsInRangeToAttack"))
	{
		// choose player to target
		if (FMath::RandRange(0.f, 1.f) < FVector::Distance(OwnerLocation, ServerPlayer->GetActorLocation()) / (FVector::Distance(OwnerLocation, ServerPlayer->GetActorLocation()) + FVector::Distance(OwnerLocation, ClientPlayer->GetActorLocation())))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentTargetPlayer", ClientPlayer);
			CurrentTargetLocation = ClientPlayer->GetActorLocation();
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentTargetPlayer", ServerPlayer);
			CurrentTargetLocation = ServerPlayer->GetActorLocation();
		}
	}
	else if (ServerPlayer->bIsSafe)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentTargetPlayer", ClientPlayer);
		CurrentTargetLocation = ClientPlayer->GetActorLocation();
	}
	else if (ClientPlayer->bIsSafe)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentTargetPlayer", ServerPlayer);
		CurrentTargetLocation = ServerPlayer->GetActorLocation();
	}

	
	
	if (bDebug)
	{
		DrawDebugSphere(OwnerCharacter->GetWorld(), CurrentTargetLocation, 30.f, 30, FColor::Red, false, 0.3f );
		DrawDebugSphere(OwnerCharacter->GetWorld(), OwnerLocation, 30.f, 30, FColor::Red, false, 0.3f );
	}
}
