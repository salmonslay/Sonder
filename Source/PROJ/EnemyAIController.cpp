// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "PROJCharacter.h"
#include "PROJGameMode.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AEnemyAIController::Initialize()
{
	if(BT)
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviourTReetorun %s"), *BT->GetName());
		RunBehaviorTree(BT);
	}
	
	//GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerOne"), UGameplayStatics::GetPlayerCharacter(this, 0));

	APROJGameMode* CurrentGameMode = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(this));
	if (CurrentGameMode != nullptr)
	{
		P1 = CurrentGameMode->GetActivePlayer(0);
		P2 = CurrentGameMode->GetActivePlayer(1);

		if (P1 != nullptr)
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerOne"), P1);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No player 1"));
		}
		if (P2 != nullptr)
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerTwo"), P2);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No player 2"));

		}
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}
