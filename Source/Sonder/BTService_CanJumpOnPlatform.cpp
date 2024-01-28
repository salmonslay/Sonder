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
				if (!OwnerCharacter->HasNavigationToTarget(BlackboardComponent->GetValueAsVector("CurrentTargetLocation")) || !OwnerCharacter->IsWithinRangeFrom(BlackboardComponent->GetValueAsVector("CurrentTargetLocation")))
				{
					if (bDebug)
					{
						//rawDebugSphere(GetWorld(), OwnerCharacter->ClosestJumpPoint, 30.f, 30, FColor::Red, false, 0.2f);
						DrawDebugSphere(GetWorld(), OwnerCharacter->AvaliableJumpPoint, 30.f, 30, FColor::Blue, false, 0.2f);
					}
					BlackboardComponent->SetValueAsBool("bIsJumping", true);
					OwnerCharacter->JumpToPoint(OwnerCharacter->AvaliableJumpPoint);
					OwnerCharacter->JumpCoolDownTimer = 0;
					OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;
				}
				else
				{
					BlackboardComponent->SetValueAsBool("bIsJumping", false);
					BlackboardComponent->ClearValue("bIsJumping");
					OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;
				}
			}
		}
	}
	BlackboardComponent->SetValueAsBool("bIsJumping", false);
	BlackboardComponent->ClearValue("bIsJumping");
	OwnerCharacter->JumpCoolDownTimer += DeltaSeconds;

}

