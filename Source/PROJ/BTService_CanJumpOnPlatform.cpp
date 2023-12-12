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

bool UBTService_CanJumpOnPlatform::CanJumpToPoint(FVector StartPoint, FVector JumpPoint)
{
	FVector GravitationalForce = OwnerCharacter->GetCharacterMovement()->GetGravityDirection();
	float CharacterSpeed = OwnerCharacter->GetCharacterMovement()->GetMaxSpeed();
	float HorizontalDistance = FVector::Dist(JumpPoint, StartPoint); 

	float TravelTime = HorizontalDistance/CharacterSpeed;
	FVector FalloffVector = GravitationalForce / 2 * FMath::Pow(TravelTime,2);

	if ((StartPoint - FalloffVector).Z <= JumpPoint.Z)
	{
		return true;
	}
	return false;
}

void UBTService_CanJumpOnPlatform::JumpToPoint(FVector StartPoint,FVector JumpPoint)
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

