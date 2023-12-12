// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CanJumpOnPlatform.h"

#include "AIController.h"
#include "ShadowCharacter.h"
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

	JumpCoolDownTimer += DeltaSeconds;
	
	if (!OwnerCharacter->bCanJump)
	{
		return;
	}

	if (OwnerCharacter->bIsJumping)
	{
		return;
	}

	if (OwnerCharacter->bCanJump && !OwnerCharacter->bIsJumping)
	{
		if (JumpCoolDownTimer >= OwnerCharacter->JumpCoolDownDuration)
		{
			JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
			JumpCoolDownTimer = 0;
			JumpCoolDownTimer += DeltaSeconds;
		}
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
	FVector OutVel;
	OwnerCharacter->GetMovementComponent()->Velocity = FVector(0.f, 0.f, 0.f);
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVel, StartPoint, JumpPoint, 0, 0.6);
	OwnerCharacter->LaunchCharacter(OutVel * JumpBoost, true, true );
	UE_LOG(LogTemp, Error, TEXT("Doing jump "));
}
















/*FVector GravitationalForce; //z is presumed to be negative.
float CharacterSpeed;
float HorizontalDistance = FVector::Dist(TargetPosition, StartPosition); 

float TravelTime = HorizontalDistance/CharacterSpeed;
FVector FalloffVector = GravitationalForce / 2 * pow(TravelTime,2);

if ((StartPosition - FalloffVector).Z <= TargetPosition.Z) HasToJump = true;*/