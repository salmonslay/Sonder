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

	if (WaitingEnemies.IsEmpty())
	{
		return;
	}
	
	for (AShadowCharacter* Enemy : WaitingEnemies)
	{
		if (Enemy)
		{
			if (Enemy->bHasLandedOnPlatform || Enemy->bHasLandedOnGround)
			{
				Enemy->bIsJumping = false;
				Enemy->bIsPerformingJump = false;
			}
			
			if (bTriggerJumpToMovablePlatform)
			{
				if (Enemy->bCanPlatformJump && !Enemy->bCanBasicJump && !Enemy->bHasLandedOnPlatform&& Enemy->bHasLandedOnGround && !Enemy->bIsJumping && !Enemy->bIsPerformingJump) // is not on moving platform
				{
					Enemy->AvaliableJumpPoint = CalculateJumpToPlatform(Enemy->GetActorLocation(), Enemy->GetActorForwardVector());
				}

				// TODO: det här är samma sak, onödig check
				else if (Enemy->bCanPlatformJump && !Enemy->bCanBasicJump && Enemy->bHasLandedOnPlatform && !Enemy->bHasLandedOnGround &&  !Enemy->bIsJumping && !Enemy->bIsPerformingJump) // is on moving platform and wants to jump off it
				{
					//UE_LOG(LogTemp, Error, TEXT("On movable platform, wants jump to point"))

					Enemy->AvaliableJumpPoint = CalculateJumpToPoint(Enemy);
				}
				else if (!Enemy->bCanPlatformJump && Enemy->bCanBasicJump && !Enemy->bHasLandedOnPlatform && Enemy->bHasLandedOnGround &&  !Enemy->bIsJumping && !Enemy->bIsPerformingJump)
				{
					//UE_LOG(LogTemp, Error, TEXT("No movable platform, wants jump to other point"))
					Enemy->AvaliableJumpPoint = CalculateJumpToPoint(Enemy);
				}
			}
			else
			{
				//TODO: Should only happen when can make basic jump and is not a movable platform jump-trigger. 
				if (Enemy->bCanBasicJump && Enemy->bHasLandedOnGround && !Enemy->bIsJumping && !Enemy->bIsPerformingJump) // ordinary jump to some point 
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

FVector AEnemyJumpTrigger::CalculateJumpToPoint(AShadowCharacter* Enemy)
{
	
	if (FVector::Distance(JumpPoint1->GetComponentLocation(), Enemy->CurrentTargetLocation) >=  FVector::Distance(JumpPoint2->GetComponentLocation(), Enemy->CurrentTargetLocation))
	{
		const float DirToJumpPointY = JumpPoint1->GetComponentLocation().Y < Enemy->GetActorLocation().Y ? -1 : 1;
		return FVector(Enemy->GetActorLocation().X, Enemy->GetActorLocation().Y + DirToJumpPointY * EnemyJumpDistance, JumpPoint1->GetComponentLocation().Z + BasicJumpZOffset);
	}
	const float DirToJumpPointY = JumpPoint2->GetComponentLocation().Y < Enemy->GetActorLocation().Y ? -1 : 1;
	return FVector(Enemy->GetActorLocation().X, Enemy->GetActorLocation().Y + DirToJumpPointY * EnemyJumpDistance, JumpPoint2->GetComponentLocation().Z + BasicJumpZOffset);
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
			Enemy->bCanPlatformJump = true;
			Enemy->bCanBasicJump = false;
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
				Enemy->bCanPlatformJump = false;
				Enemy->bCanBasicJump = true;
			}
		}
	}
}

