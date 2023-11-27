// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsInAttackRange.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "PROJCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

class APROJCharacter;

UBTService_IsInAttackRange::UBTService_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");
}

void UBTService_IsInAttackRange::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_IsInAttackRange::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_IsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	APROJCharacter* PlayerToAttack;
	UObject* PlayerObject = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject("PlayerToAttack");

	if (PlayerObject == nullptr)
	{
		return;
	}
	PlayerToAttack = Cast<APROJCharacter>(PlayerObject);
	if (PlayerToAttack)
	{
		PlayerToAttackLocation = PlayerToAttack->GetActorLocation();
	}
	
	const float HeightDiff = FMath::Abs(OwnerLocation.Z - PlayerToAttackLocation.Z);
	FVector DirectionToPlayer = PlayerToAttackLocation -OwnerLocation;
	DirectionToPlayer.Normalize();
    
	
    float DotProduct = FVector::DotProduct(FVector(0, 1, 0), DirectionToPlayer);
	
    float Angle = FMath::Acos(FMath::Abs(DotProduct)) * (180.0f / PI);

    // Check if the angle is within the specified range
	if (HeightDiff > OwnerCharacter->MaxAttackHeightDifference && Angle > OwnerCharacter->MaxAttackAngle)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", false);
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", GenerateNewStartAttackPoint());
	}
	else
	{
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(OwnerCharacter);

		FHitResult Hit;
		bool bHit;

		if (bDebug)
		{
			bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation + OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
		}
		else
		{
			bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerLocation, OwnerLocation +OwnerCharacter->GetActorForwardVector() * OwnerCharacter->LaserRange, ObjectTypeQueries, true, ActorsToIgnore, EDrawDebugTrace::None, Hit, true, FLinearColor::Blue, FLinearColor::Green, OwnerCharacter->PerformAttackDuration );
		}

		if (bHit)
		{
			APROJCharacter* PlayerHit = Cast<APROJCharacter>(Hit.GetActor());
			if (PlayerHit)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", true);
				OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", OwnerLocation);
			}
			else
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", false);
				//OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", GenerateNewStartAttackPoint());
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsInRangeToAttack", true);
			OwnerComp.GetBlackboardComponent()->SetValueAsVector("StartAttackPosition", OwnerLocation);
		}
	}
}

bool UBTService_IsInAttackRange::StartAttackPointValid(const FVector& PointToCheck)
{
	const float HeightDiff = FMath::Abs(OwnerLocation.Z - PlayerToAttackLocation.Z);
	FVector DirectionToPlayer = PlayerToAttackLocation -OwnerLocation;
	DirectionToPlayer.Normalize();
    
	
	float DotProduct = FVector::DotProduct(FVector(0, 1, 0), DirectionToPlayer);
	
	float Angle = FMath::Acos(FMath::Abs(DotProduct)) * (180.0f / PI);

	// Check if the angle is within the specified range
	if (HeightDiff > OwnerCharacter->MaxAttackHeightDifference && Angle > OwnerCharacter->MaxAttackAngle)
	{
		return false;
	}
	return true;
}

FVector UBTService_IsInAttackRange::GenerateNewStartAttackPoint()
{
	// Generate a random height 
	const float RandomHeight = FMath::FRandRange(PlayerToAttackLocation.Z - OwnerCharacter->MaxAttackHeightDifference, PlayerToAttackLocation.Z + OwnerCharacter->MaxAttackHeightDifference);
	const float RandomAngle = FMath::FRandRange(0.0f, FMath::DegreesToRadians(OwnerCharacter->MaxAttackAngle));
	
	const float HorizontalDistance = FMath::Sqrt(FMath::Square(PlayerToAttackLocation.X - OwnerLocation.X) + FMath::Square(PlayerToAttackLocation.Y - OwnerLocation.Y));
	const float HorizontalOffset = HorizontalDistance * FMath::Tan(RandomAngle);
	
	const float NewX = PlayerToAttackLocation.X + HorizontalOffset;
	const float NewY = PlayerToAttackLocation.Y + HorizontalOffset;

	// Calculate rand new position
	FVector NewPosition = FVector(NewX, NewY, PlayerToAttackLocation.Z + RandomHeight);
	//FVector NewPosition = FVector(PlayerToAttackPosition.X, PlayerToAttackPosition.Y , OwnerLocation.Z +RandomHeight);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), NewPosition, 20.f, 30, FColor::Cyan, false, 0.2f, 0, 5);
	}
	return NewPosition;
}
