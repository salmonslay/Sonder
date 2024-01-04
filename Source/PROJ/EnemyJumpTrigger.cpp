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


FVector AEnemyJumpTrigger::RequestJumpLocation(const FVector &EnemyLoc, const FVector &CurrentTargetLocation, const bool bIsOnPlatform)
{
	if (bIsOverlappingWithMovingPlatform && !bIsOnPlatform)
	{
		return CalculateJumpToPlatform(EnemyLoc, CurrentTargetLocation);
	}
	if (bIsOverlappingWithMovingPlatform && bIsOnPlatform)
	{
		return CalculatePointClosetsToTarget(EnemyLoc, CurrentTargetLocation);
	}
	return CalculatePointFurthestFromEnemy(EnemyLoc);
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
		const UNavigationPath* NavigationPath = Navigation->FindPathToLocationSynchronously(GetWorld(), JumpPoint1Loc, JumpPoint2Loc);

		if(NavigationPath == nullptr)
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