// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyJumpTrigger.h"

#include "MovingPlatform.h"
#include "ShadowCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	if (!bIsOverlappingWithMovingPlatform) return;
	if (!OverlappingGround) return;
	
	for (AShadowCharacter* Enemy : WaitingEnemies)
	{
		if (Enemy)
		{
			if (Enemy->bHasLandedOnPlatform || Enemy->bHasLandedOnGround)
			{
				Enemy->bIsJumping = false;
			}
			
			if (Enemy->bCanJumpToPlatform && !Enemy->bCanJumpFromPlatform && !Enemy->bIsJumping)
            {
            
                Enemy->AvaliableJumpPoint = CalculateJumpToPlatform(Enemy->GetActorLocation(), Enemy->GetActorForwardVector());
                UE_LOG(LogTemp, Error, TEXT("want to jump to point %f, %f, %f"), Enemy->AvaliableJumpPoint.X, Enemy->AvaliableJumpPoint.Y, Enemy->AvaliableJumpPoint.Z);
                DrawDebugSphere(GetWorld(),Enemy->AvaliableJumpPoint, 30.f, 24, FColor::Blue, false, 5.f);
            }
			
			
			/*
			else if (!Enemy->bCanJumpToPlatform && Enemy->bCanJumpFromPlatform && !Enemy->bIsJumping)
			{
				Enemy->AvaliableJumpPoint = CalculateJumpFromPlatform(Enemy->GetActorLocation(), Enemy->GetActorForwardVector());
			}
			
			*/
		}
	}
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

FVector AEnemyJumpTrigger::CalculateJumpToPlatform(const FVector& EnemyLocation, const FVector& EnemyForwardVector) // forward vector * Jumpdistance
{
	FVector Origin;
	FVector Extent;
	OverlappingPlatform->GetActorBounds(true, Origin, Extent);
	return FVector(EnemyLocation.X, EnemyLocation.Y + EnemyForwardVector.Y * EnemyJumpDistance, Origin.Z);
	
}

FVector AEnemyJumpTrigger::CalculateJumpFromPlatform(const FVector& EnemyLocation, const FVector& EnemyForwardVector)
{
	FVector Origin;
	FVector Extent;
	OverlappingGround->GetActorBounds(true, Origin, Extent);
	return FVector(EnemyLocation.X, EnemyLocation.Y + EnemyForwardVector.Y * EnemyJumpDistance, Origin.Z);
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
			if (!Enemy->bHasLandedOnPlatform)
			{
				UE_LOG(LogTemp, Error, TEXT("Allowing jump to platform"));
				Enemy->bCanJumpToPlatform = true;
				//Enemy->bCanJumpFromPlatform = false;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Allowing jumping from platform"));

				//Enemy->bCanJumpToPlatform = false;
				Enemy->bCanJumpFromPlatform = true;
			}
		}
	
	}
}

void AEnemyJumpTrigger::DenyJump()  // runs on overlap end with moving platform, if enemy is standing on a platform - allow jump to ground, if not - allow jump to platform
{
	//if (!bIsOverlappingWithMovingPlatform)
	//{
		if (!WaitingEnemies.IsEmpty())
		{
			for (AShadowCharacter* Enemy : WaitingEnemies)
			{
				if (Enemy)
				{
					if (!Enemy->bHasLandedOnPlatform)
					{
						UE_LOG(LogTemp, Error, TEXT("Denying jump to platform"));
						Enemy->bCanJumpToPlatform = false;
						//Enemy->bCanJumpFromPlatform = true;
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Denying jump from platform"));
						//Enemy->bCanJumpToPlatform = true;
						Enemy->bCanJumpFromPlatform = false;
					}
					
				}
			}
		}
	//}
}

