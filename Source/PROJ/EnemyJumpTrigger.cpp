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

	JumpPoint1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump point 1"));
	JumpPoint1->SetupAttachment(Bounds);

	JumpPoint2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump point 2"));
	JumpPoint2->SetupAttachment(Bounds);
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

	//if (!bIsOverlappingWithMovingPlatform) return;
	//if (!OverlappingGround) return; // bort
	
	for (AShadowCharacter* Enemy : WaitingEnemies)
	{
		if (Enemy)
		{
			if (Enemy->bHasLanded || Enemy->bHasLandedOnGround)
			{
				Enemy->bIsJumping = false;
				Enemy->bIsPerformingJump = false;
			}
			
			if (bTriggerJumpToMovablePlatform )
			{
				if (Enemy->bCanJump && !Enemy->bHasLanded && !Enemy->bIsJumping) // is not on moving platform
				{
					Enemy->AvaliableJumpPoint = CalculateJumpToPlatform(Enemy->GetActorLocation(), Enemy->GetActorForwardVector());
					//UE_LOG(LogTemp, Error, TEXT("want to jump to jumppoint %f, %f, %f"), Enemy->AvaliableJumpPoint.X, Enemy->AvaliableJumpPoint.Y, Enemy->AvaliableJumpPoint.Z);
					//DrawDebugSphere(GetWorld(),Enemy->AvaliableJumpPoint, 30.f, 24, FColor::Blue, false, 5.f);
				}
				else if (Enemy->bCanJump && Enemy->bHasLanded && !Enemy->bIsJumping) // is on moving platform and wants to jump off it
				{
					Enemy->AvaliableJumpPoint = CalculateJumpToPoint(Enemy->GetActorLocation());
					//UE_LOG(LogTemp, Error, TEXT("want to jump to jumppoint"));
					//DrawDebugSphere(GetWorld(),Enemy->AvaliableJumpPoint, 30.f, 24, FColor::Blue, false, 5.f);
				}
			}
			else
			{
				
			}
			
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

FVector AEnemyJumpTrigger::CalculateJumpToPoint(const FVector& EnemyLocation)
{
	if (FVector::Distance(EnemyLocation, JumpPoint1->GetComponentLocation()) > FVector::Distance(EnemyLocation, JumpPoint2->GetComponentLocation()))
	{
		return JumpPoint1->GetComponentLocation();
	}
	//UE_LOG(LogTemp, Error, TEXT("want to jump to point 2"));
	//DrawDebugSphere(GetWorld(),JumpPoint2->GetComponentLocation(), 30.f, 24, FColor::Yellow, false, 5.f);
	return JumpPoint2->GetComponentLocation();
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

			Enemy->bCanJump = true;
			/*
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
			*/
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

					Enemy->bCanJump = false;
					/*
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
					*/
				}
			}
		}
	//}
}

