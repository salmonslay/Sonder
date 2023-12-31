// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SeparateFromCollision.h"

#include "AIController.h"
#include "FlyingEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_SeparateFromCollision::UBTService_SeparateFromCollision()
{
	NodeName = TEXT("SeparateFromCollision");
}

void UBTService_SeparateFromCollision::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);

	CollisionObjectParams = FCollisionObjectQueryParams();
	for (auto Iter = ObjectTypeQueries.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel& Channel = UCollisionProfile::Get()->ConvertToCollisionChannel(false, *Iter);
		CollisionObjectParams.AddObjectTypesToQuery(Channel);
	}
}

void UBTService_SeparateFromCollision::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AFlyingEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	TotalSeparationVector = FVector::ZeroVector;
	CollisionCounter = 0;

	// Make a sphere from cats location as large as defined radius
	const FVector MyLocation = OwnerCharacter->GetActorLocation();
	const FCollisionShape CheckSphereShape = FCollisionShape::MakeSphere(CollisionCheckRadius); 
	TArray<FOverlapResult> OverlapResults;

	if (bDebug) DrawDebugSphere(GetWorld(), MyLocation, CollisionCheckRadius, 24, FColor::Red, false, .5f);

	// check if sphere overlaps with any rats
	const bool bOverlaps = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		MyLocation,
		FQuat::Identity,
		CollisionObjectParams,
		CheckSphereShape);

	if(bOverlaps)
	{
		for(FOverlapResult Overlap : OverlapResults)
		{
			const AActor* OverlappingActor = Overlap.GetActor();
			if (OverlappingActor)
			{
				if (OverlappingActor && Cast<AFlyingEnemyCharacter>(OverlappingActor) && (OverlappingActor->GetName() != OwnerCharacter->GetName()))
				{
					CollisionCounter++;
					TotalSeparationVector += OverlappingActor->GetActorLocation() - OwnerLocation;
					if (bDebug)
					{
						UE_LOG(LogTemp, Error, TEXT("Overlapping %s"), *OverlappingActor->GetName());
					}
				}
			}
		}

		if (CollisionCounter - 1 > 0)
		{
			const FVector CurrentSeparationForce = CalculateTotalSeparationForce(TotalSeparationVector);
			OwnerCharacter->GetCharacterMovement()->Velocity += CurrentSeparationForce * 100.f;
		}
	}
}

FVector UBTService_SeparateFromCollision::CalculateTotalSeparationForce(FVector& Separation) const
{
	if (CollisionCounter > 0)
	{
		Separation /= CollisionCounter;
		Separation *= -1;
		Separation.Normalize();
		return Separation;
	}
	return FVector::ZeroVector;
}
