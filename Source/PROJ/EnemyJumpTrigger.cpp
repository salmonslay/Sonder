// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyJumpTrigger.h"

#include "MovingPlatform.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "ShadowCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnemyJumpTrigger::AEnemyJumpTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Bounds->SetupAttachment(RootComponent);

	JumpPoint1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump point 1"));
	JumpPoint1->SetupAttachment(Bounds);

	JumpPoint2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump point 2"));
	JumpPoint2->SetupAttachment(Bounds);
}

// Called when the game starts or when spawned
void AEnemyJumpTrigger::BeginPlay()
{
	Super::BeginPlay();

	JumpPoint1Loc = JumpPoint1->GetComponentLocation();
	JumpPoint2Loc = JumpPoint2->GetComponentLocation();
}

// Called every frame
void AEnemyJumpTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	if (WaitingEnemies.IsEmpty())
	{
		return;
	}
	
	for (AShadowCharacter* Enemy : WaitingEnemies)
	{
		if (Enemy)
		{
			FVector EnemyLocation = Enemy->GetActorLocation();

	
			if (bTriggerJumpToMovablePlatform) // TODO:Doesnt need this, can be same trigger for both after fix
			{
				if (bIsOverlappingWithMovingPlatform && Enemy->bCanBasicJump && Enemy->bHasLandedOnGround && !Enemy->bIsJumping && !Enemy->bIsPerformingJump)
				{
					Enemy->AvaliableJumpPoint = CalculateJumpToPlatform(EnemyLocation);
				}

				else if (!bIsOverlappingWithMovingPlatform && Enemy->bCanBasicJump && Enemy->bHasLandedOnGround && !Enemy->bIsJumping && !Enemy->bIsPerformingJump)
				{
					// TODO: Check if is on platform, if is on platform it should jump to closest point to target but if its not it should jump to the point furthest away from it
					// TODO: Go to closest movable platform in BT if hasnt path to player.

					if (!IsLeveledWithJumpPoints(EnemyLocation))
					{
						//UE_LOG(LogTemp, Warning, TEXT("Not IsLeveled with platform, own location"));
						Enemy->AvaliableJumpPoint = CalculatePointClosetsToTarget(EnemyLocation, Enemy->CurrentTargetLocation);
					}
					else
					{
						//UE_LOG(LogTemp, Warning, TEXT("IsLeveled with platform"));
						Enemy->AvaliableJumpPoint = CalculatePointFurthestFromEnemy(EnemyLocation);
					}
				}
				// TODO: If is overlapping with moving platform, and is grounded, and is not jumping/performing jump
			}
			
			else
			{
				//TODO: Should only happen when can make basic jump and is not a movable platform jump-trigger. 
				if (Enemy->bCanBasicJump && Enemy->bHasLandedOnGround && !Enemy->bIsJumping && !Enemy->bIsPerformingJump) // ordinary jump to some point 
				{
					Enemy->AvaliableJumpPoint = CalculatePointFurthestFromEnemy(EnemyLocation);
				}
			}
		}
	}
	*/

}

void AEnemyJumpTrigger::RemoveWaitingEnemy(AShadowCharacter* EnemyToRemove)
{
	if (!EnemyToRemove) return;
	
	if (WaitingEnemies.Contains(EnemyToRemove))
	{
		WaitingEnemies.Remove(EnemyToRemove);
	}
}

void AEnemyJumpTrigger::AddWaitingEnemy(AShadowCharacter* EnemyToAdd)
{
	if (!EnemyToAdd) return;
	
	if (!WaitingEnemies.Contains(EnemyToAdd))
	{
		WaitingEnemies.Add(EnemyToAdd);
	}
}

FVector AEnemyJumpTrigger::RequestJumpLocation(const FVector &EnemyLoc, const FVector &CurrentTargetLocation, const bool bIsOnPlatform)
{
	if (bTriggerJumpToMovablePlatform)
	{
		if (bIsOverlappingWithMovingPlatform && !bIsOnPlatform)
		{
			return CalculateJumpToPlatform(EnemyLoc);
			
		}
		if (bIsOverlappingWithMovingPlatform && bIsOnPlatform)
		{
			return CalculatePointClosetsToTarget(EnemyLoc, CurrentTargetLocation);
		}
	}
	return CalculatePointFurthestFromEnemy(EnemyLoc);
}

FVector AEnemyJumpTrigger::CalculateJumpToPlatform(const FVector& EnemyLocation) const // forward vector * Jumpdistance
{
	// TODO: Check if platform is closer to player than enemy, only jump if that is true SHOULD NOT BE HERE; MAYBE IN SERVICE
	if (!OverlappingPlatform)
	{
		return FVector::ZeroVector;
	}
	FVector Origin;
	FVector Extent;
	OverlappingPlatform->GetActorBounds(true, Origin, Extent);

	// Jump left if platform's origin is to the left and vice versa with right 
	const float DirToPlatformY = Origin.Y < EnemyLocation.Y ? -1 : 1;
	
	return FVector(EnemyLocation.X, EnemyLocation.Y + DirToPlatformY * EnemyJumpDistance, Origin.Z + PlatformJumpZOffset);
}

FVector AEnemyJumpTrigger::CalculatePointClosetsToTarget(const FVector& EnemyLocation, const FVector& CurrentTargetLocation) const 
{
	if (FVector::Distance(JumpPoint1Loc, CurrentTargetLocation) <=  FVector::Distance(JumpPoint2Loc, CurrentTargetLocation))
	{
		const float DirToJumpPointY = JumpPoint1Loc.Y < EnemyLocation.Y ? -1 : 1;
		return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, JumpPoint1Loc.Z);
	}
	const float DirToJumpPointY = JumpPoint2Loc.Y < EnemyLocation.Y ? -1 : 1;
	return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, JumpPoint2Loc.Z);
}

FVector AEnemyJumpTrigger::CalculatePointFurthestFromEnemy(const FVector& EnemyLocation) const
{
	if (FVector::Distance(JumpPoint1Loc, EnemyLocation) >=  FVector::Distance(JumpPoint2Loc, EnemyLocation))
	{
		const float DirToJumpPointY = JumpPoint1Loc.Y < EnemyLocation.Y ? -1 : 1;
		return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, JumpPoint1Loc.Z + BasicJumpZOffset);
	}
	const float DirToJumpPointY = JumpPoint2Loc.Y < EnemyLocation.Y ? -1 : 1;
	return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, JumpPoint2Loc.Z + BasicJumpZOffset);
}

void AEnemyJumpTrigger::AllowJump() // runs on overlap begin with moving platform, if enemy has once jumped on a platform - allow jump to ground, if not - allow jump to platform
{
	if (WaitingEnemies.IsEmpty())
	{
		return;
	}
	
	for (AShadowCharacter* Enemy : WaitingEnemies)
	{
		if (Enemy)
		{
			//Enemy->bCanPlatformJump = true;
			//Enemy->bCanBasicJump = false;
			Enemy->JumpCoolDownTimer = Enemy->JumpCoolDownDuration;
		}
	}
}

void AEnemyJumpTrigger::DenyJump()  // runs on overlap end with moving platform, if enemy is standing on a platform - allow jump to ground, if not - allow jump to platform
{
	if (!WaitingEnemies.IsEmpty())
	{
		for (AShadowCharacter* Enemy : WaitingEnemies)
		{
			if (Enemy)
			{
				//Enemy->bCanPlatformJump = false;
				//Enemy->bCanBasicJump = true;
			}
		}
	}
}

bool AEnemyJumpTrigger::HasPathBetweenPoints() const
{
	const UNavigationSystemV1* Navigation = UNavigationSystemV1::GetCurrent(GetWorld());

	if (ensure(IsValid(Navigation))) {
		const UNavigationPath* NavigationPath = Navigation->FindPathToLocationSynchronously(GetWorld(), JumpPoint1Loc, JumpPoint2Loc);

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

bool AEnemyJumpTrigger::IsLeveledWithJumpPoints(const FVector &EnemyLoc) const
{
	//UE_LOG(LogTemp, Error, TEXT(" EnemyLoc= %f, Jumppoints loc = %f, %f"), EnemyLoc.Z, JumpPoint1Loc.Z, JumpPoint2Loc.Z );
	return FMath::IsNearlyEqual(EnemyLoc.Z, JumpPoint1Loc.Z, 5.f) && FMath::IsNearlyEqual(EnemyLoc.Z, JumpPoint2Loc.Z, 5.f);
}
