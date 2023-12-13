// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CanJumpOnPlatform.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NavigationSystemTypes.h"
#include "ShadowCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_CanJumpOnPlatform::UBTService_CanJumpOnPlatform()
{
	NodeName = TEXT("CanJumpOnPlatform");
}

void UBTService_CanJumpOnPlatform::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);

	JumpCoolDownTimer = 0.f;
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
	
	if (!OwnerCharacter->bCanJump)
	{
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
		return;
	}

	if (OwnerCharacter->bIsJumping || OwnerCharacter->bIsPerformingJump )
	{
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", false);
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->ClearValue("bIsJumping");
		return;
	}

	if (OwnerCharacter->bCanJump && !OwnerCharacter->bIsJumping)
	{
		if (OwnerCharacter->JumpCoolDownTimer >= OwnerCharacter->JumpCoolDownDuration)
		{
			
			if (OwnerCharacter->AvaliableJumpPoint != FVector::ZeroVector)
			{
				if (!HasNavigationTo(OwnerLocation, OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsVector("CurrentMoveTarget")))
				{
					OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", true);
					JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
					OwnerCharacter->JumpCoolDownTimer = 0;
					OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;
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
	OwnerCharacter->MakeJump();
	FVector OutVel;
	OwnerCharacter->GetMovementComponent()->Velocity = FVector(0.f, 0.f, 0.f);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVel, StartPoint, JumpPoint, 0, 0.6);
	OwnerCharacter->GetCharacterMovement()->AddImpulse(OutVel * JumpBoost);
	if (bDebug)
	{
		DrawDebugSphere(GetWorld(),JumpPoint, 30.f, 24, FColor::Blue, false, 2.f);
		UE_LOG(LogTemp, Error, TEXT("Doing jump "));
	}
}

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


bool UBTService_CanJumpOnPlatform::HasNavigationTo(const FVector &StartPoint, const FVector &CurrentTargetPoint) const
{
	const UNavigationSystemV1* Navigation = UNavigationSystemV1::GetCurrent(GetWorld());

	if (ensure(IsValid(Navigation))) {
		const UNavigationPath* NavigationPath = Navigation->FindPathToLocationSynchronously(GetWorld(), StartPoint, CurrentTargetPoint);

		if(ensure(NavigationPath != nullptr) == false)
		{
			return false;
		}
		const bool IsNavigationValid = NavigationPath->IsValid();
		const bool IsNavigationNotPartial = NavigationPath->IsPartial() == false;
		const bool IsNavigationSuccessful = IsNavigationValid && IsNavigationNotPartial;
		return IsNavigationSuccessful;
	}
	return false;
}


