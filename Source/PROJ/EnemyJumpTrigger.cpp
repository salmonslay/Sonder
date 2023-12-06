// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyJumpTrigger.h"

#include "MovingPlatform.h"
#include "ShadowCharacter.h"
#include "Components/BoxComponent.h"

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
	
}

// Called every frame
void AEnemyJumpTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

FVector AEnemyJumpTrigger::CalculateJumpEndPoint(const FVector& EnemyLocation)
{
	if (bIsOverlappingWithMovingPlatform && OverlappingPlatform != nullptr)
	{
		FVector Origin;
		FVector Extent;
		OverlappingPlatform->GetActorBounds(true, Origin, Extent);
		return FVector(EnemyLocation.X, (Origin + (Extent/2)-EnemyJumpOffset).Y, (Origin + (Extent/2)-EnemyJumpOffset).Z);
	}
	return FVector::ZeroVector;
}

void AEnemyJumpTrigger::AllowJumpToPlatform()
{
	if (bIsOverlappingWithMovingPlatform)
	{
		if (!WaitingEnemies.IsEmpty())
		{
			for (AShadowCharacter* Enemy : WaitingEnemies)
			{
				if (Enemy)
				{
					Enemy->bCanJumpFromPlatform = true;
					Enemy->AvaliableJumpPoint = CalculateJumpEndPoint(Enemy->GetActorLocation());
				}
			}
		}
	}
}

void AEnemyJumpTrigger::DenyJumpToPlatform()
{
	if (!bIsOverlappingWithMovingPlatform)
	{
		if (!WaitingEnemies.IsEmpty())
		{
			for (AShadowCharacter* Enemy : WaitingEnemies)
			{
				if (Enemy)
				{
					Enemy->bCanJumpFromPlatform = false;
				}
			}
		}
	}
}

