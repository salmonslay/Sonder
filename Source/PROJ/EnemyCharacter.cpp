// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "EnemyAIController.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EnemyHealthComponent.h"


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

	GetWorldTimerManager().SetTimer(InitializerTimerHandle, this, &AEnemyCharacter::InitializeController, 2, false, 2);
	
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
		if (EventInstigator->IsLocalController())
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy killed by listen server player"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy killed by client player"));
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
	
}