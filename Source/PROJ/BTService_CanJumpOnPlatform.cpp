// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CanJumpOnPlatform.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "NavigationSystemTypes.h"
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

			//TODO: 
			if (OwnerCharacter->AvaliableJumpPoint != FVector::ZeroVector)
			{
				OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool("bIsJumping", true);
				JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
				OwnerCharacter->JumpCoolDownTimer = 0;
				OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;
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

bool UBTService_CanJumpOnPlatform::CheckPathToPlayer(const FVector &StartPoint, const FVector &JumpPoint)
{
	/*
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		AController* EnemyController = OwnerCharacter->GetController();
		// Check if there's a path between owner character and current target location
		const ANavigationData* NavData = Cast<ANavigationData>(NavSystem->GetNavDataForProps(EnemyController->GetNavAgentPropertiesRef()));
		if (NavData)
		{
			FNavAgentProperties AgentProperties;
			FPathFindingQuery Query = FPathFindingQuery(EnemyController, *NavData, StartPoint, JumpPoint);
			
			//bool bPathExists = NavSystem->TestPathSync(Query, EPathFindingMode::Hierarchical);
			FNavPathSharedPtr NavPath;
			FPathFindingResult PathResult;
			bool bPathExists = NavSystem->FindPathSync(NavData, Query, StartPoint, JumpPoint, PathResult, nullptr);


			// Path exists, check for holes in the NavMesh
			if (bPathExists && NavPath.IsValid())
			{
				TArray<FVector> PathPoints;
				NavPath->GetPathPoints(PathPoints);

				for (const FVector& PathPoint : PathPoints)
				{
					// Check if each point on the path is navigable
					if (!NavData->ProjectPointToNavigation(PathPoint, PathPoint))
					{
						// Point is not navigable, suggesting a potential hole or broken NavMesh area
						// Handle accordingly or log the location for debugging
					}
				}
			}
			if (bPathExists)
			{
				
				if (Result.IsSuccessful())
				{
					// No holes in the NavMesh between AI character and player character
					// Proceed with actions for direct path
				}
				else
				{
					// Holes found in the NavMesh between AI character and player character
					// Handle accordingly
				}
			}
			else
			{
				// No path found between AI character and player character
				// Handle accordingly
			}
		}
	}
	*/
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


