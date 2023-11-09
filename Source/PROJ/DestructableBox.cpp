// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableBox.h"

#include "BaseHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADestructableBox::ADestructableBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UBaseHealthComponent>(TEXT("HealthComponent"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

}

void ADestructableBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ADestructableBox, HealthComponent);
}

// Called when the game starts or when spawned
void ADestructableBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADestructableBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



float ADestructableBox::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = HealthComponent->TakeDamage(DamageApplied);

	if (HealthComponent->IsDead())
	{
		KillMe();
	}

	return DamageApplied;
}

void ADestructableBox::KillMe()
{
	OnDeath();
}

