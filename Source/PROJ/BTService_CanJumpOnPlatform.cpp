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

	// TODO: when can jump aka enemy is overlapping with trigger and moving platform is there, jump

	if (OwnerCharacter->bCanJumpFromPlatform) //TODO: Chech if enemy is jumping, in that case dont do jump
	{
		if (bDebug) DrawDebugSphere(GetWorld(), OwnerCharacter->AvaliableJumpPoint, 20, 24, FColor::Red, false, .5f);
	}
	/*
	OwnerLocation = OwnerCharacter->GetActorLocation();

	// Make a sphere from cats location as large as defined radius
	const FVector MyLocation = OwnerCharacter->GetActorLocation();
	const FCollisionShape CheckSphereShape = FCollisionShape::MakeSphere(RadiusToDetectPlatform); 
	FCollisionObjectQueryParams Params = FCollisionObjectQueryParams();

	// Add other channels maybe?
	Params.AddObjectTypesToQuery(ECC_GameTraceChannel7); // Moving platform


	TArray<FOverlapResult> OverlapResults;

	if (bDebug) DrawDebugSphere(GetWorld(), MyLocation, RadiusToDetectPlatform, 24, FColor::Black, false, .5f);

	// check if sphere overlaps with any rats
	bool bOverlaps = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		MyLocation,
		FQuat::Identity,
		Params,
		CheckSphereShape);
	
	if(bOverlaps)
	{
		for(FOverlapResult Overlap : OverlapResults)
		{
			// if overlap with platform is found, check its bounds and if its close enough to allow jump
			AMovingPlatform* MovingPlatform = Cast<AMovingPlatform>(Overlap.GetActor());
			if (MovingPlatform)
			{
				FVector Origin;
				FVector Extent;
				MovingPlatform->GetActorBounds(true, Origin, Extent);
				FVector JumpPoint = FVector(OwnerLocation.X, (Origin + Extent/2).Y, (Origin + Extent/2).Z);
				
				
				//point on platform closest to character
				if (CanJumpToPoint(OwnerLocation, JumpPoint))
				{
					JumpToPoint(OwnerLocation,JumpPoint);
					break;
				}
			}
		}
	}
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
	FVector OutVel;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), OutVel, StartPoint, JumpPoint);
	OwnerCharacter->LaunchCharacter(OutVel * JumpBoost, true, true );
}
















/*FVector GravitationalForce; //z is presumed to be negative.
float CharacterSpeed;
float HorizontalDistance = FVector::Dist(TargetPosition, StartPosition); 

float TravelTime = HorizontalDistance/CharacterSpeed;
FVector FalloffVector = GravitationalForce / 2 * pow(TravelTime,2);

if ((StartPosition - FalloffVector).Z <= TargetPosition.Z) HasToJump = true;*/