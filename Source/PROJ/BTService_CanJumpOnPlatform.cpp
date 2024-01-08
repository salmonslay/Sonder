// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CanJumpOnPlatform.h"

#include "AIController.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_CanJumpOnPlatform::UBTService_CanJumpOnPlatform()
{
	NodeName = TEXT("CanJumpOnPlatform");
}

void UBTService_CanJumpOnPlatform::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	BlackboardComponent = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	
	if (BlackboardComponent == nullptr) return;
	
	if (OwnerCharacter->bIsJumping || OwnerCharacter->bIsPerformingJump )
	{
		BlackboardComponent->SetValueAsBool("bIsJumping", true);
	}
	
	OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;
	
	if (OwnerCharacter->bCanBasicJump) // removed check can jump to platform
	{
		if (OwnerCharacter->JumpCoolDownTimer >= OwnerCharacter->JumpCoolDownDuration)
		{
			if (OwnerCharacter->AvaliableJumpPoint != FVector::ZeroVector)
			{
				if (!OwnerCharacter->HasNavigationToTarget(BlackboardComponent->GetValueAsVector("CurrentTargetLocation")))
				{
					BlackboardComponent->SetValueAsBool("bIsJumping", true);

					DrawDebugSphere(GetWorld(), OwnerCharacter->ClosestJumpPoint, 30.f, 30, FColor::Red, false, 0.2f);
					DrawDebugSphere(GetWorld(), OwnerCharacter->AvaliableJumpPoint, 30.f, 30, FColor::Blue, false, 0.2f);

					JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
					OwnerCharacter->JumpCoolDownTimer = 0;
					OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Has a valid path and therefore no jumpy :(("))
				}
			}
		}
	}
	BlackboardComponent->SetValueAsBool("bIsJumping", false);
	BlackboardComponent->ClearValue("bIsJumping");
}


void UBTService_CanJumpOnPlatform::JumpToPoint(const FVector &StartPoint, const FVector &JumpPoint) const 
{
	OwnerCharacter->bIsJumping = true;
	FVector OutVel;
	OwnerCharacter->GetMovementComponent()->Velocity = FVector(0.f, 0.f, 0.f);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVel, StartPoint, JumpPoint, 0, 0.6);
	OwnerCharacter->GetCharacterMovement()->AddImpulse(OutVel.GetSafeNormal() * JumpBoost);
	OwnerCharacter->MakeJump();
}

/*
bool UBTService_CanJumpOnPlatform::CheckPathToPlayer(const FVector &StartPoint, const FVector &CurrentTargetPoint)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("No navsystem lol"));
		return false;
	}
	{
		AController* EnemyController = OwnerCharacter->GetController();
		// Check if there's a path between owner character and current target location

		const auto Path = UNavigationSystemV1::FindPathToLocationSynchronously(this, StartPoint, CurrentTargetPoint, EnemyController);
		
		if(Path->IsPartial() || Path->IsUnreachable())
		{
			UE_LOG(LogTemp, Error, TEXT("Path is partial or unreachable, should return false"));
		}

		if (!Path)
		{
			UE_LOG(LogTemp, Error, TEXT("No path found"));
			return false;			
		}

		FVector QueryExtents = OwnerCharacter->GetSimpleCollisionCylinderExtent();
		TArray<FNavPathPoint> NavPoints = Path->GetPath()->GetPathPoints();

		for (const FNavPathPoint NavPathPoint : NavPoints)
		{
			// Check if each point on the path is navigable
			FNavLocation NavLocation;
			if (!NavSystem->ProjectPointToNavigation(NavPathPoint.Location, NavLocation, QueryExtents))
			{
				return false;
			}
		}
		return true;
	}
}
*/


