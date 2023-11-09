// Fill out your copyright notice in the Description page of Project Settings.


#include "NewPlayerHealthComponent.h"

#include "PROJCharacter.h"


UNewPlayerHealthComponent::UNewPlayerHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Note: Tick is turned off 
}

void UNewPlayerHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APROJCharacter>(GetOwner()); 
}


float UNewPlayerHealthComponent::TakeDamage(const float DamageAmount)
{
	// Run only on the local player 
	if(!Player->IsLocallyControlled())
		return Super::TakeDamage(DamageAmount);

	const float DamageTaken = Super::TakeDamage(DamageAmount);

	ServerRPCDamageTaken(DamageTaken); 

	return DamageTaken; 
}

void UNewPlayerHealthComponent::ServerRPCDamageTaken_Implementation(const float DamageTaken)
{
	// Run only on server 
	if(!Player->HasAuthority())
		return; 
	
	MulticastRPCDamageTaken(DamageTaken); 
}

void UNewPlayerHealthComponent::MulticastRPCDamageTaken_Implementation(const float DamageTaken)
{
	Player->OnDamageTaken(DamageTaken); 
}

void UNewPlayerHealthComponent::IDied()
{
	Super::IDied();

	// Run only on the local player, place this before Super call? 
	if(!Player->IsLocallyControlled())
		return; 
	
	ServerRPCPlayerDied(); 
}

void UNewPlayerHealthComponent::ServerRPCPlayerDied_Implementation()
{
	// Run only on server 
	if(!Player->HasAuthority())
		return;

	MulticastRPCPlayerDied(); 
}

void UNewPlayerHealthComponent::MulticastRPCPlayerDied_Implementation()
{
	Player->OnPlayerDied(); 
}
