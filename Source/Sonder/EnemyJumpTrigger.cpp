// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyJumpTrigger.h"

#include "EnemyJumpPoint.h"
#include "MovingPlatform.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "ShadowCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AEnemyJumpTrigger::AEnemyJumpTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Bounds->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyJumpTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	for (const auto JumpPoint : JumpPoints)
	{
		ensure (JumpPoint != nullptr);
		JumpPointLocations.Add(JumpPoint->GetActorLocation());
		JumpPoint->JumpTrigger = this;
	}
}

// Called every frame
void AEnemyJumpTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!JumpPointLocations.IsEmpty())
	{
		return;
	}
	for (const auto JumpPoint : JumpPoints)
	{
		ensure (JumpPoint != nullptr);
		JumpPointLocations.Add(JumpPoint->GetActorLocation());
		JumpPoint->JumpTrigger = this;
	}
}

void AEnemyJumpTrigger::AddOverlappingPlatform(AMovingPlatform* PlatformToAdd)
{
	if (ensure (PlatformToAdd!= nullptr))
	{
		OverlappingPlatforms.Add(PlatformToAdd);
	}
}

void AEnemyJumpTrigger::RemoveOverlappingPlatform(AMovingPlatform* PlatformToRemove)
{
	if (ensure (PlatformToRemove != nullptr))
	{
		OverlappingPlatforms.Remove(PlatformToRemove);
	}
	if (OverlappingPlatforms.IsEmpty())
	{
		bIsOverlappingWithMovingPlatform = false;
	}
}


FVector AEnemyJumpTrigger::RequestJumpLocation(const FVector &EnemyLoc, const FVector &CurrentTargetLocation, const FVector &ClosestJumpPoint, const bool bIsOnPlatform)
{
	if (bIsOverlappingWithMovingPlatform && !bIsOnPlatform)
	{
		return CalculateJumpToPlatform(EnemyLoc, CurrentTargetLocation);
	}
	if (bIsOverlappingWithMovingPlatform && bIsOnPlatform)
	{
		return CalculatePointClosetsToTarget(EnemyLoc, CurrentTargetLocation);
	}
	if (!bIsOverlappingWithMovingPlatform && JumpPointLocations.Num() <= 1) // if trigger only has tqo jump points, calculate furthest one without regarding for closeness to player
	{
		return CalculatePointFurthestFromEnemy(EnemyLoc);
	}
	if (!bIsOverlappingWithMovingPlatform && JumpPointLocations.Num() >= 2) // if trigger has more than 2 jump points
	{
  		return CalculateAccessiblePointFurthestFromEnemy(EnemyLoc, ClosestJumpPoint, CurrentTargetLocation);
	}
	return FVector::ZeroVector;
 }

FVector AEnemyJumpTrigger::CalculateJumpToPlatform(const FVector& EnemyLocation, const FVector& CurrentTargetLocation)  // forward vector * Jumpdistance
{
	// TODO: Check if platform is closer to player than enemy, only jump if that is true SHOULD NOT BE HERE; MAYBE IN SERVICE
	
	if (OverlappingPlatforms.Num() > 1)
	{
		OverlappingPlatform = SelectPlatform(EnemyLocation, CurrentTargetLocation);
	}
	else
	{
		OverlappingPlatform = OverlappingPlatforms[0];
	}
	// Choose platform to jump to
	if (!OverlappingPlatform.Get() && ensure(OverlappingPlatform != nullptr))
	{
		return FVector::ZeroVector;
	}
	FVector Origin;
	FVector Extent;
	OverlappingPlatform.Get()->GetActorBounds(true, Origin, Extent);

	// Jump left if platform's origin is to the left and vice versa with right 
	const float DirToPlatformY = Origin.Y < EnemyLocation.Y ? -1 : 1;
	
	return FVector(EnemyLocation.X, EnemyLocation.Y + DirToPlatformY * EnemyJumpDistance, Origin.Z + PlatformJumpZOffset);
}

TWeakObjectPtr<AMovingPlatform> AEnemyJumpTrigger::SelectPlatform(const FVector& EnemyLocation, const FVector& CurrentTargetLocation) const
{
	TWeakObjectPtr<AMovingPlatform> BestPtr;
	float BestScore = 0.f;
	
	for (const auto &Platform : OverlappingPlatforms)
	{
		if (Platform.IsValid())
		{
			const float DistanceToPlayer = FVector::Distance(EnemyLocation, CurrentTargetLocation);
			// Calculate the dot product of the direction to player and platform movement vector
			const float DotPlatformToTarget = FVector::DotProduct(CurrentTargetLocation, Platform.Get()->GetMovementDelta().GetSafeNormal());

			// Calculate the dot product of the direction to player and player's movement direction
			const float DotEnemyToTarget = FVector::DotProduct(EnemyLocation, CurrentTargetLocation);

			// Calculate score based on direction towards the player and platform movement, update best actor if needed

			const float ComboDot = DotPlatformToTarget + DotEnemyToTarget;
			const float Score = ComboDot + (1.0f / DistanceToPlayer);
			if (Score > BestScore)
			{
				BestScore = Score;
				BestPtr = Platform;
			}
		}
	}
	return BestPtr;
}

FVector AEnemyJumpTrigger::CalculatePointClosetsToTarget(const FVector& EnemyLocation, const FVector& CurrentTargetLocation) const 
{
	float MinDistance = 5000000000.f;
	FVector Point;

	for (FVector JumpPointLoc : JumpPointLocations)
	{
		if (FVector::Distance(JumpPointLoc, CurrentTargetLocation) <= MinDistance)
		{
			MinDistance = FVector::Distance(JumpPointLoc, CurrentTargetLocation);
			Point = JumpPointLoc;
		}
	}
	DrawDebugSphere(GetWorld(), Point, 30.f, 6, FColor::Orange, false, 0.2f );
	const float DirToJumpPointY = Point.Y < EnemyLocation.Y ? -1 : 1;
	return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, Point.Z);
}

FVector AEnemyJumpTrigger::CalculatePointFurthestFromEnemy(const FVector& EnemyLocation) const
{
	float MaxDistance = 0.f;
	FVector Point;
	
	for (FVector JumpPointLoc : JumpPointLocations)
	{
		if (FVector::Distance(JumpPointLoc, EnemyLocation) >= MaxDistance)
		{
			MaxDistance = FVector::Distance(JumpPointLoc, EnemyLocation);
			Point = JumpPointLoc;
		}
	}
	const float DirToJumpPointY = Point.Y < EnemyLocation.Y ? -1 : 1;
	return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, Point.Z + BasicJumpZOffset);
}

FVector AEnemyJumpTrigger::CalculateAccessiblePointFurthestFromEnemy(const FVector& EnemyLocation, const FVector& ClosestPointToEnemy,  const FVector& CurrentTargetLocation )
{
	FVector ReachablePoint;

	TArray<FVector> PossibleJumpPoints;
	// add all point locations that are further away then the closest one
	for (FVector JumpPointLoc : JumpPointLocations)
	{
		if (FVector::Distance(JumpPointLoc, EnemyLocation) >= FVector::Distance(ClosestPointToEnemy, EnemyLocation))
		{
			PossibleJumpPoints.Add(JumpPointLoc);
		}
	}

	// Find point closest to player that is reachable from those points
	// perform a raycast to see if the location is reachable, choosing the location furthest away from enemy that is reachable
	float ReachableMinDistanceToPlayer = 50000000.f;
	float MinDistanceToPlayer = 500000000.f;
	FVector DefaultPoint = FVector::ZeroVector;

	for (FVector PossibleJumpLoc : PossibleJumpPoints)
	{
		if (CanReachJumpPoint(EnemyLocation, PossibleJumpLoc))
		{
			if (FVector::Distance(PossibleJumpLoc, CurrentTargetLocation) <= ReachableMinDistanceToPlayer)
			{
				ReachableMinDistanceToPlayer = FVector::Distance(PossibleJumpLoc, CurrentTargetLocation);
				ReachablePoint = PossibleJumpLoc;
			}
		}
		else
		{
			if (FVector::Distance(PossibleJumpLoc, CurrentTargetLocation) <= MinDistanceToPlayer)
			{
				MinDistanceToPlayer = FVector::Distance(PossibleJumpLoc, CurrentTargetLocation);
				DefaultPoint = PossibleJumpLoc;
			}
		}
	}

	// if there are no jump points that enemy has sight of, default to jumping to the one closest to current target player
	if (ReachablePoint == FVector::ZeroVector)
	{
		const float DirToJumpPointY = DefaultPoint.Y < EnemyLocation.Y ? -1 : 1;
		//DrawDebugSphere(GetWorld(), DefaultPoint, 30.f, 6, FColor::Yellow, false, 0.2f );
		return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, DefaultPoint.Z + BasicJumpZOffset);
	}
	const float DirToJumpPointY = ReachablePoint.Y < EnemyLocation.Y ? -1 : 1;
	//DrawDebugSphere(GetWorld(),  FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, ReachablePoint.Z + BasicJumpZOffset), 30.f, 6, FColor::Yellow, false, 0.2f );
	return FVector(EnemyLocation.X, EnemyLocation.Y + DirToJumpPointY * EnemyJumpDistance, ReachablePoint.Z + BasicJumpZOffset);
}

bool AEnemyJumpTrigger::CanReachJumpPoint(const FVector& PointFrom, const FVector& PointTo)
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	//return !UKismetSystemLibrary::LineTraceSingleForObjects(this, PointFrom, PointTo, LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FColor::Red, FColor::Green, 0.3f);

	return !UKismetSystemLibrary::LineTraceSingleForObjects(this, PointFrom, PointTo, LineTraceObjects, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	
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
			Enemy->JumpCoolDownTimer = Enemy->JumpCoolDownDuration;
		}
	}
}

bool AEnemyJumpTrigger::HasPathBetweenPoints() const
{
	const UNavigationSystemV1* Navigation = UNavigationSystemV1::GetCurrent(GetWorld());

	if ( !UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		return false;
	}
	if (ensure(IsValid(Navigation))) {

		if (!JumpPointLocations.IsEmpty())
		{
			const FVector First = JumpPointLocations[0];
			bool IsNavigationSuccessful = false;

			for (FVector Point : JumpPointLocations)
			{
				const UNavigationPath* NavigationPath = Navigation->FindPathToLocationSynchronously(GetWorld(), First, Point);

				if(NavigationPath == nullptr)
				{
					return false;
				}
				const bool IsNavigationValid = NavigationPath->IsValid();
				const bool IsNavigationNotPartial = NavigationPath->IsPartial() == false;
				IsNavigationSuccessful = IsNavigationValid && IsNavigationNotPartial;
				if (IsNavigationSuccessful)
				{
					return IsNavigationSuccessful;
				}
			}
			return IsNavigationSuccessful;
		}
		return false;
	}
	return false;
}

/*
bool AEnemyJumpTrigger::IsLeveledWithJumpPoints(const FVector &EnemyLoc) const
{
	//UE_LOG(LogTemp, Error, TEXT(" EnemyLoc= %f, Jumppoints loc = %f, %f"), EnemyLoc.Z, JumpPoint1Loc.Z, JumpPoint2Loc.Z );
	//return FMath::IsNearlyEqual(EnemyLoc.Z, JumpPoint1Loc.Z, 5.f) && FMath::IsNearlyEqual(EnemyLoc.Z, JumpPoint2Loc.Z, 5.f);
}
*/

/*for (AActor* Actor : Actors)
	{
		if (!Actor || !Actor->IsValidLowLevelFast()) continue;

		FVector ActorLocation = Actor->GetActorLocation();

		// Calculate the direction from the actor to the player
		FVector DirectionToPlayer = (PlayerLocation - ActorLocation).GetSafeNormal();

		// Calculate the dot product of the direction to player and player's movement direction
		float DotProduct = FVector::DotProduct(DirectionToPlayer, PlayerMovementDelta.GetSafeNormal());

		// Calculate the distance between the actor and the player
		float DistanceToPlayer = FVector::Distance(ActorLocation, PlayerLocation);

		// Calculate a score based on direction and distance (you can adjust weights as needed)
		float Score = DotProduct + (1.0f / DistanceToPlayer);

		// Update the best actor if this actor has a better score
		if (Score > BestScore)
		{
			BestScore = Score;
			BestActor = Actor;
		}
	}

	return BestActor;*/