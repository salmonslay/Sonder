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

void UBTService_CanJumpOnPlatform::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UBTService_CanJumpOnPlatform::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}

void UBTService_CanJumpOnPlatform::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;

	// TESTING
	// this should really not be here and its horrid but there are a lot of things that needs to change
	if (!OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool("bHasValidPath") && OwnerCharacter->bHasLandedOnPlatform
		&& !FMath::IsNearlyEqual(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector("CurrentMoveTarget").Y,OwnerLocation.Y, 3 ))
	{
		if (OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool("bHasValidPath"))
		{
			UE_LOG(LogTemp, Error, TEXT("Has valid path"));
		}
		UE_LOG(LogTemp, Error, TEXT("IsOnPlatform"));
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsOnPlatform", true);
	}
	else
	{
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsOnPlatform", false);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsOnPlatform");
	}
	
	if (!OwnerCharacter->bCanPlatformJump && !OwnerCharacter->bCanBasicJump)
	{
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
		return;
	}

	if (OwnerCharacter->bIsJumping || OwnerCharacter->bIsPerformingJump )
	{
		// this feels wrong but its right, should rename the bb-key
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
		return;
	}

	if ((OwnerCharacter->bCanPlatformJump || OwnerCharacter->bCanBasicJump) && !OwnerCharacter->bIsJumping) // check if can jump || can jump to platform
	{
		if (OwnerCharacter->JumpCoolDownTimer >= OwnerCharacter->JumpCoolDownDuration)
		{
			//UE_LOG(LogTemp, Error, TEXT("Jump cooldown is done"))
			if (OwnerCharacter->AvaliableJumpPoint != FVector::ZeroVector)
			{
				//  && OwnerCharacter->PointCloserToPlayer(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector("CurrentMoveTarget"))
				if (!OwnerCharacter->HasNavigationToTarget(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector("CurrentMoveTarget")))
				{
					//UE_LOG(LogTemp, Error, TEXT("Wants to jump from service"))
					OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", true);
					JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
					OwnerCharacter->JumpCoolDownTimer = 0;
					OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;
				}
				else
				{
					//UE_LOG(LogTemp, Error, TEXT("Wants to jump but has navigation"))
					OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
					OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
				}
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("Wants to jump but jump point is zero"))
				OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
				OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
			}
		}
		else
		{
			OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
			OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
		}
	}
	else
	{
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
	}
}


void UBTService_CanJumpOnPlatform::JumpToPoint(const FVector &StartPoint, const FVector &JumpPoint) const 
{
	OwnerCharacter->bIsJumping = true;
	OwnerCharacter->bIsPerformingJump = true;
	FVector OutVel;
	OwnerCharacter->GetMovementComponent()->Velocity = FVector(0.f, 0.f, 0.f);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVel, StartPoint, JumpPoint, 0, 0.6);
	OwnerCharacter->GetCharacterMovement()->AddImpulse(OutVel.GetSafeNormal() * JumpBoost);
	OwnerCharacter->MakeJump();

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(),JumpPoint, 30.f, 24, FColor::Blue, false, 2.f);
		UE_LOG(LogTemp, Error, TEXT("Doing jump "));
	}
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


