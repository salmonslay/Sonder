// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CanAttackPlayer.h"

#include "AIController.h"
#include "EnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_CanAttackPlayer::UBTService_CanAttackPlayer()
{
	NodeName = TEXT("CanAttackPlayer");
}

void UBTService_CanAttackPlayer::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_CanAttackPlayer::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_CanAttackPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwnerComp.GetBlackboardComponent()->ClearValue("PlayerToAttack");
	//OwnerComp.GetBlackboardComponent()->ClearValue("CurrentTargetPlayer");

	OwnerComp.GetBlackboardComponent()->SetValueAsBool("bFoundPlayerWithinAttackRadius", false);
	OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	// Make a sphere from cats location as large as defined radius
	const FVector MyLocation = OwnerCharacter->GetActorLocation();
	const FCollisionShape CheckSphereShape = FCollisionShape::MakeSphere(RadiusToDamagePlayer); 
	FCollisionObjectQueryParams Params = FCollisionObjectQueryParams();
	Params.AddObjectTypesToQuery(ECC_Pawn);
	TArray<FOverlapResult> OverlapResults;

	if (bDebug) DrawDebugSphere(GetWorld(), MyLocation, RadiusToDamagePlayer, 24, FColor::Black, false, .5f);

	// check if sphere overlaps with any rats
	bool bOverlaps = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		MyLocation,
		FQuat::Identity,
		Params,
		CheckSphereShape);
	
	if(bOverlaps)
	{
		for(FOverlapResult Overlap : OverlapResults)
		{
			APROJCharacter* PlayerToAttack = Cast<APROJCharacter>(Overlap.GetActor());
			// if overlap is found, set values in bb and break
			if (PlayerToAttack && IsValid(PlayerToAttack) && !PlayerToAttack->bIsSafe)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject("PlayerToAttack", PlayerToAttack);
				OwnerComp.GetBlackboardComponent()->SetValueAsObject("CurrentTargetPlayer", PlayerToAttack);
				
				if (bDebug)
				{
					UE_LOG(LogTemp, Warning, TEXT("Found player within attack radius"))
					DrawDebugSphere(GetWorld(), PlayerToAttack->GetActorLocation(), 30.f, 24, FColor::Blue, false, .2f);
				}	
				
				OwnerComp.GetBlackboardComponent()->SetValueAsBool("bFoundPlayerWithinAttackRadius", true);
				break;
			}
		}
	}
}
