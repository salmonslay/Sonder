// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CanJumpOnPlatform.h"

#include "AIController.h"
#include "ShadowCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_CanJumpOnPlatform::UBTService_CanJumpOnPlatform()
{
	NodeName = TEXT("CanJumoOnPlatform");
}

void UBTService_CanJumpOnPlatform::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (OwnerComp.GetAIOwner() == nullptr) return;

	OwnerCharacter = Cast<AShadowCharacter>(OwnerComp.GetAIOwner()->GetCharacter());

	if (OwnerCharacter == nullptr) return;

	OwnerLocation = OwnerCharacter->GetActorLocation();

	if (!OwnerCharacter->bCanJumpFromPlatform && !OwnerCharacter->bCanJumpToPlatform)
	{
		return;
	}

	if (OwnerCharacter->bIsJumping)
	{
		return;
	}
	//  if both are false, do whatever it is you do
	if (OwnerCharacter->bCanJumpToPlatform && !OwnerCharacter->bCanJumpFromPlatform && !OwnerCharacter->bIsJumping)
	{
		JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
	}
	
	
	/*
	if (OwnerCharacter->bCanJumpToPlatform && !OwnerCharacter->bIsJumping && !OwnerCharacter->bHasLandedOnPlatform) // inside trigger with a moving platform, not jumping and doesnt stand on a moving platform
	{
		// Om jumppoint till eller från plattform är närmare Current target player än ens egna location, hoppa
		// TODO: Jump delay, for anim. then make jump
		
		//JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
	}
	if (OwnerCharacter->bHasLandedOnPlatform) // Standing on moving platform,
	{
		OwnerCharacter->bIsJumping = false;
		OwnerCharacter->bCanJumpToPlatform = false; // om has landed on platform, player can jump from platform = true on overlap end
		OwnerCharacter->bCanJumpFromPlatform = true;
	}

	
	if (!OwnerCharacter->bCanJumpToPlatform &&OwnerCharacter->bCanJumpFromPlatform && !OwnerCharacter->bIsJumping && OwnerCharacter->bHasLandedOnPlatform) // inside trigger with a moving platform, not jumping and doesnt stand on a moving platform
	{
	// Om jumppoint till eller från plattform är närmare Current target player än ens egna location, hoppa
	// TODO: Jump delay, for anim. then make jump
		//JumpToPoint(OwnerLocation, OwnerCharacter->AvaliableJumpPoint);
		//DrawDebugSphere(GetWorld(),OwnerCharacter->AvaliableJumpPoint, 30.f, 24, FColor::Green, false, 1.f);
	}

	// när man gjort hoppet, rör dig mot spelaren

	*/
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
	FVector OutVel;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVel, StartPoint, JumpPoint);
	OwnerCharacter->LaunchCharacter(OutVel * JumpBoost, false, false );

	UE_LOG(LogTemp, Error, TEXT("Doing jump "));

}
















/*FVector GravitationalForce; //z is presumed to be negative.
float CharacterSpeed;
float HorizontalDistance = FVector::Dist(TargetPosition, StartPosition); 

float TravelTime = HorizontalDistance/CharacterSpeed;
FVector FalloffVector = GravitationalForce / 2 * pow(TravelTime,2);

if ((StartPosition - FalloffVector).Z <= TargetPosition.Z) HasToJump = true;*/