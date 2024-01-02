// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	LastKnownLocation = GetActorLocation();
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveDelta = GetActorLocation() - LastKnownLocation;
	LastKnownLocation = GetActorLocation();
}

float AMovingPlatform::GetMoveDirectionZ() const 
{
	return MoveDelta.Z;
}

float AMovingPlatform::GetMoveDirectionY() const
{
	return MoveDelta.Y;
}

FVector AMovingPlatform::GetMovementDelta() const
{
	return MoveDelta;
}

bool AMovingPlatform::IsMovingTowardLocation(const FVector& To) const
{
	// Calculate distance to the target location along both Z and Yaxis
	const FVector Loc = GetActorLocation();
	const float ZDistance = To.Z - Loc.Z;
	const float YDistance = To.Z - Loc.Z;


	if ((MoveDelta.Y > 0 && YDistance > 0) || (MoveDelta.Y < 0 && YDistance < 0))
	{
		// Moving in the Y axis direction and the target is in the same direction
		if ((MoveDelta.Z > 0 && ZDistance > 0) || (MoveDelta.Z < 0 && ZDistance < 0))
		{
			// Moving in the Z axis direction and the target is in the same direction
			return true;
		}
	}
	return false;
}
