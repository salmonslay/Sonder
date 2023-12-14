// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "EnemyAIController.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EnemyHealthComponent.h"
#include "Grid.h"
#include "PROJCharacter.h"
#include "RobotBaseState.h"
#include "RobotStateMachine.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyHealthComponent = CreateDefaultSubobject<UEnemyHealthComponent>(TEXT("EnemyHealthComponent"));

	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
}



// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	if(!GetController())
	{
		SpawnDefaultController();
	}
	GetWorldTimerManager().SetTimer(InitializerTimerHandle, this, &AEnemyCharacter::InitializeController, 2, false, 2);

	CheckIfOverlappingWithGrid();

	SpawnPosition = GetActorLocation();
}

void AEnemyCharacter::InitializeController()
{
	if(bIsControllerInitialized) return;
	UE_LOG(LogTemp, Warning, TEXT("Start initializing controller"));
	bIsControllerInitialized = true;
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if(AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("HAsController"));

		AIController->Initialize();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Controller cast failed"));
	}
}


// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::OnRep_Stunned()
{
	if(bIsStunned)
		OnStunnedEvent();
	else
		OnUnstunnedEvent();
}

void AEnemyCharacter::OnRep_ChargingAttack()
{
	if (bIsChargingAttack)
	{
		OnChargingAttackEvent(ChargeAttackDuration);
	}
}

void AEnemyCharacter::OnRep_Attack()
{
	if (bIsAttacking)
	{
		OnPerformAttackEvent(PerformAttackDuration);
	}
}

void AEnemyCharacter::Stun(const float Duration)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if(bIsStunned)
		{
			if(Duration > StunnedDuration)
			{
				StunnedDuration = Duration;
			}
		}
		else
		{
			bIsStunned = true;
			bIsChargingAttack = false;
			bIsAttacking = false;
			bIsIdle = false;
			OnStunnedEvent();
			StunnedDuration = Duration;
		}
		//GetWorldTimerManager().SetTimer(StunnedTimerHandle, this, &AEnemyCharacter::Idle, Duration, false, -1.f);
	}
}

void AEnemyCharacter::ChargeAttack()
{
	if (GetLocalRole() == ROLE_Authority && !bIsChargingAttack && !bIsStunned)
	{
		bIsChargingAttack = true;
		bIsAttacking = false;
		//bIsStunned = false;
		bIsIdle = false;
		OnChargingAttackEvent(ChargeAttackDuration);
		//GetWorldTimerManager().SetTimer(ChargeAttackTimerHandle, this, &AEnemyCharacter::Attack, ChargeAttackDuration, false, -1.f);
	}
}

void AEnemyCharacter::Attack()
{
	if (GetLocalRole() == ROLE_Authority && !bIsAttacking && !bIsStunned)
	{
		bIsAttacking = true;
		bIsChargingAttack = false;
		//bIsStunned = false;
		bIsIdle = false;
		OnPerformAttackEvent(PerformAttackDuration);
		//GetWorldTimerManager().ClearTimer(ChargeAttackTimerHandle);
		//GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemyCharacter::Idle, PerformAttackDuration, false, -1.f);
	}
}

void AEnemyCharacter::Idle()
{
	if (GetLocalRole() == ROLE_Authority && !bIsIdle && !bIsStunned)
	{
		//bIsStunned = false;
		bIsAttacking = false;
		bIsChargingAttack = false;
		bIsIdle = true;
		//GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		//GetWorldTimerManager().ClearTimer(StunnedTimerHandle);
	}
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	// Increase damage by eventual Robot damage multiplier 
	if(const auto Robot = Cast<ARobotStateMachine>(DamageCauser))
		DamageAmount *= Robot->FindComponentByClass<URobotBaseState>()->GetDamageBoostMultiplier();
	
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (EnemyHealthComponent)
	{
		DamageApplied = EnemyHealthComponent->TakeDamage(DamageApplied);

		if(DamageAmount >= StaggeredThreshold)
		{
			Stun(StaggeredDuration);	
		}

		if (DamageCauser && IsValid(DamageCauser))
		{
			APROJCharacter* TempDamageCauser = Cast<APROJCharacter>(DamageCauser);
			if (TempDamageCauser != nullptr)
			{
				LatestDamageCauser = TempDamageCauser;
				bHasBeenAttacked = true; //TODO:Set to false somewhere, maybe in a service
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy health component is nullptr"));
	}
	return DamageApplied;
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// redundant now that has own health comp??
	DOREPLIFETIME(AEnemyCharacter, EnemyHealthComponent);
	DOREPLIFETIME(AEnemyCharacter, bIsStunned);
	DOREPLIFETIME(AEnemyCharacter, bIsChargingAttack);
	DOREPLIFETIME(AEnemyCharacter, bIsAttacking);
	DOREPLIFETIME(AEnemyCharacter, bIsIdle);
}

void AEnemyCharacter::InitializeControllerFromManager()
{
	if(bIsControllerInitialized) return;
	SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw + 90.0f, 0.0f));
	InitializeController();
}

void AEnemyCharacter::SetGridPointer(AGrid* GridPointer)
{
	if(GridPointer)
	{
		PathfindingGrid = GridPointer;
	}
}

AGrid* AEnemyCharacter::GetGridPointer() const 
{
	return PathfindingGrid;
}


APROJCharacter* AEnemyCharacter::GetLatestDamageCauser()
{
	return LatestDamageCauser;
}

bool AEnemyCharacter::GetHasBeenAttacked() const 
{
	return bHasBeenAttacked;
}

void AEnemyCharacter::CheckIfOverlappingWithGrid()
{
	TArray<AActor*> OverlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, AGrid::StaticClass());
	
	if (!OverlappingActors.IsEmpty())
	{
		for (AActor* OverlappingActor : OverlappingActors)
		{
			AGrid* TempGrid = Cast<AGrid>(OverlappingActor);
			if (TempGrid != nullptr && IsValid(TempGrid))
			{
				PathfindingGrid = TempGrid;
			}
		}
	}
}


void AEnemyCharacter::KillMe()
{
	GetWorldTimerManager().ClearTimer(StunnedTimerHandle);
	GetWorldTimerManager().ClearTimer(ChargeAttackTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

