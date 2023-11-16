// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "EnemyAIController.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EnemyHealthComponent.h"
#include "Grid.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyHealthComponent = CreateDefaultSubobject<UEnemyHealthComponent>(TEXT("EnemyHealthComponent"));

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

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (EnemyHealthComponent)
	{
		DamageApplied = EnemyHealthComponent->TakeDamage(DamageApplied);
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

void AEnemyCharacter::CheckIfOverlappingWithGrid()
{
	TArray<AActor*> OverlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, AEnemyCharacter::StaticClass());
	
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
