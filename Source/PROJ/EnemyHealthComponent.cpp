// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHealthComponent.h"

#include "EnemyCharacter.h"
UEnemyHealthComponent::UEnemyHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Note: Tick is turned off 
}

void UEnemyHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyCharacter = Cast<AEnemyCharacter>(GetOwner());
}


float UEnemyHealthComponent::TakeDamage(const float DamageAmount)
{
	// Run only on the local player 
	if(!EnemyCharacter->IsLocallyControlled())
		return Super::TakeDamage(DamageAmount);

	const float DamageTaken = Super::TakeDamage(DamageAmount);

	
	ServerRPCDamageTaken(DamageTaken); 

	return DamageTaken; 
}

void UEnemyHealthComponent::ServerRPCDamageTaken_Implementation(const float DamageTaken)
{
	// Run only on server 
	if(!EnemyCharacter->HasAuthority())
		return; 

	CurrentHealth -= DamageTaken;
	if (CurrentHealth <= 0)
	{
		IDied();
	}

	MulticastRPCDamageTaken(DamageTaken); 
}

void UEnemyHealthComponent::MulticastRPCDamageTaken_Implementation(const float DamageTaken)
{
	EnemyCharacter->OnTakenDamageEvent(DamageTaken); 
}

void UEnemyHealthComponent::IDied()
{
	Super::IDied();

	// Run only on the local player, place this before Super call? 
	if(!EnemyCharacter->IsLocallyControlled())
		return; 
	
	ServerRPCPlayerDied(); 
}

void UEnemyHealthComponent::ServerRPCPlayerDied_Implementation()
{
	// Run only on server 
	if(!EnemyCharacter->HasAuthority())
		return;

	MulticastRPCPlayerDied(); 
}

void UEnemyHealthComponent::MulticastRPCPlayerDied_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Multicast died"));
	EnemyCharacter->OnDeathEvent(); 
}

