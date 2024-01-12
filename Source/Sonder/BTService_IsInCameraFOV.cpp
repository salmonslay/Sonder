// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsInCameraFOV.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "SonderGameState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

UBTService_IsInCameraFOV::UBTService_IsInCameraFOV()
{
	NodeName = TEXT("IsInCameraFOV");
}

void UBTService_IsInCameraFOV::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	if (OwnerCharacter == nullptr)
	{
		return;
	}

	OwnerLocation = OwnerCharacter->GetActorLocation();

	SGS = Cast<ASonderGameState>(GetWorld()->GetGameState());

	if (SGS == nullptr)
	{
		return;
	}

	if (SGS->IsInCameraFieldOfView(OwnerLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsWithinCameraFOV", true);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsWithinCameraFOV", false);
	}
}
