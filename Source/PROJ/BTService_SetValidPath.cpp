// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetValidPath.h"

#include "AIController.h"
#include "MovingPlatform.h"
#include "PROJCharacter.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBTService_SetValidPath::UBTService_SetValidPath()
{
	NodeName = TEXT("SetValidPath"); 
}

void UBTService_SetValidPath::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const FVector CurrentTarget = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKeyCurrentMoveTarget.SelectedKeyName);

	APawn* Owner = OwnerComp.GetAIOwner()->GetPawn();

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;
	
	OwnerLocation = OwnerCharacter->GetActorLocation();

	BlackboardComponent = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	
	if (BlackboardComponent == nullptr) return;

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z + HeightDifferenceToMarkInvalid), 30.f, 30, FColor::Red, false, 1.f );
		DrawDebugSphere(GetWorld(), FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z - HeightDifferenceToMarkInvalid), 30.f, 30, FColor::Red, false, 1.f );

	}

	VerifyGroundStatus(OwnerCharacter);

	if (OwnerCharacter->bHasLandedOnPlatform && !FMath::IsNearlyEqual(BlackboardComponent->GetValueAsVector("CurrentMoveTarget").Z,OwnerLocation.Z, 3 ))
	{
		BlackboardComponent->SetValueAsBool("bIsOnPlatform", true);
	}
	else
	{
		BlackboardComponent->SetValueAsBool("bIsOnPlatform", false);
		BlackboardComponent->ClearValue("bIsOnPlatform");
	}
	
	if (FMath::Abs(OwnerLocation.Z - CurrentTarget.Z) <=HeightDifferenceToMarkInvalid)
	{
		BlackboardComponent->SetValueAsBool("bIsLeveledWithCurrentTarget", true);
		if (HasLineOfSightToPlayer(OwnerCharacter, CurrentTarget))
		{
			BlackboardComponent->SetValueAsBool("bHasLineOfSightToCurrentTarget", true);
		}
		else
		{
			BlackboardComponent->SetValueAsBool("bHasLineOfSightToCurrentTarget", false);
			BlackboardComponent->ClearValue("bHasLineOfSightToCurrentTarget");
		}
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, true); 
		return;
	}
	
	if(FMath::Abs(OwnerLocation.Z - CurrentTarget.Z) > HeightDifferenceToMarkInvalid)
	{
		BlackboardComponent->SetValueAsBool("bIsLeveledWithCurrentTarget", false);
		OwnerComp.GetBlackboardComponent()->ClearValue("bIsLeveledWithCurrentTarget");
		if (!OwnerCharacter->bIsPerformingJump && HasLineOfSightToPlayer(OwnerCharacter, CurrentTarget))
		{
			BlackboardComponent->SetValueAsBool("bHasLineOfSightToCurrentTarget", true);
		}
		else
		{
			BlackboardComponent->SetValueAsBool("bHasLineOfSightToCurrentTarget", false);
			BlackboardComponent->ClearValue("bHasLineOfSightToCurrentTarget");
		}
		SetPathIsInvalid(OwnerComp);
		return; 
	}
	
	if (!OwnerCharacter->HasNavigationToTarget(CurrentTarget))
	{
		SetPathIsInvalid(OwnerComp);
		return; 
	}
	
	// Path valid 
	BlackboardComponent->SetValueAsBool(BlackboardKey.SelectedKeyName, true);

	//Has line of sight to player
	BlackboardComponent->SetValueAsBool("bHasLineOfSightToCurrentTarget", false);
	BlackboardComponent->ClearValue("bHasLineOfSightToCurrentTarget");
}

bool UBTService_SetValidPath::HasLineOfSightToPlayer(AShadowCharacter* Owner, const FVector &CurrentPlayerTarget) const
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);
		
	if (bDebug)
	{
		return !UKismetSystemLibrary::LineTraceSingleForObjects(this, Owner->GetActorLocation(), CurrentPlayerTarget, LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FColor::Red, FColor::Blue, 10.f);
	}
	return !UKismetSystemLibrary::LineTraceSingleForObjects(this, Owner->GetActorLocation(), CurrentPlayerTarget, LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

}


void UBTService_SetValidPath::SetPathIsInvalid(UBehaviorTreeComponent& OwnerComp) const
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, false); 
	OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.SelectedKeyName);
}

void UBTService_SetValidPath::VerifyGroundStatus(AShadowCharacter* Owner)
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	bool bHit;
	if (bDebug)
	{
		bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, OwnerLocation, FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z - 90.f), LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FColor::Red, FColor::Blue, 10.f);
	}
	bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, OwnerLocation, FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z - 90.f), LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	if (bHit)
	{
		if (AMovingPlatform* Other = Cast<AMovingPlatform>(HitResult.GetActor()))
		{
			if(Other->GetClass() == MovingPlatformClass)
			{
				OwnerCharacter->bHasLandedOnPlatform = true;
				OwnerCharacter->bHasLandedOnGround = true;
			}
		}
		else
		{
			OwnerCharacter->bHasLandedOnPlatform = false;
			OwnerCharacter->bHasLandedOnGround = true;
		}
	}
	else
	{
		//OwnerCharacter->bHasLandedOnPlatform = false;
		//	OwnerCharacter->bHasLandedOnGround = false;
		//OwnerCharacter->bIsPerformingJump = true;
	}
}
