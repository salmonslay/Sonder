// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyJumpTrigger.h"

#include "MovingPlatform.h"
#include "ShadowCharacter.h"
#include "SonderGameState.h"
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

	if (WaitingEnemies.IsEmpty()) return;
	
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
				if (Enemy->bCanJump && !Enemy->bHasLanded && Enemy->bHasLandedOnGround && !Enemy->bIsJumping && !Enemy->bIsPerformingJump) // is not on moving platform
				{
					Enemy->AvaliableJumpPoint = CalculateJumpToPlatform(Enemy->GetActorLocation(), Enemy->GetActorForwardVector());
				}
				else if (Enemy->bCanJump && Enemy->bHasLanded && !Enemy->bHasLandedOnGround &&  !Enemy->bIsJumping && !Enemy->bIsPerformingJump) // is on moving platform and wants to jump off it
				{
					Enemy->AvaliableJumpPoint = CalculateJumpToPoint(Enemy);
				}
			}
			else
			{
				
				if (Enemy->bCanJump && Enemy->bHasLandedOnGround && !Enemy->bIsJumping && !Enemy->bIsPerformingJump) // ordinary jump to some point 
				{
					Enemy->AvaliableJumpPoint = CalculateJumpToPoint( Enemy);
				}
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

FVector AEnemyJumpTrigger::CalculateJumpToPoint(AShadowCharacter* Enemy)
{
	// choose jump point that is closest to enemy's current target
	if (FVector::Distance(JumpPoint1->GetComponentLocation(), Enemy->CurrentTargetLocation) <=  FVector::Distance(JumpPoint2->GetComponentLocation(), Enemy->CurrentTargetLocation))
	{
		return JumpPoint1->GetComponentLocation();
	}
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
			Enemy->JumpCoolDownTimer = Enemy->JumpCoolDownDuration;
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
				}
			}
		}
	//}
}

