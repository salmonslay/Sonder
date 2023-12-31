// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "EnemyCharacter.h"
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

	if (Cast<AEnemyCharacter>(GetOwner()))
	{
		GetBlackboardComponent()->SetValueAsFloat(TEXT("ChargingTime"),Cast<AEnemyCharacter>(GetOwner())->ChargeAttackDuration);
		GetBlackboardComponent()->SetValueAsVector(TEXT("RetreatLocation"), Cast<AEnemyCharacter>(GetOwner())->RetreatLocation);
	}

	const APROJGameMode* CurrentGameMode = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(this));
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

APROJCharacter* AEnemyAIController::GetPlayerFromController(const int Index) const
{
	if (Index == 0)
	{
		return P1;
	}
	return P2;
}
