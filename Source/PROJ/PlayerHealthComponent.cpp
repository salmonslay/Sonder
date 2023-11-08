// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHealthComponent.h"

#include "PROJCharacter.h"

void UPlayerHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APROJCharacter>(GetOwner()); 
}

float UPlayerHealthComponent::TakeDamage(const float DamageAmount)
{
	// Run only on the local player 
	if(!Player->IsLocallyControlled())
		return Super::TakeDamage(DamageAmount);

	const float DamageTaken = Super::TakeDamage(DamageAmount);

	ServerRPCDamageTaken(DamageTaken); 

	return DamageTaken; 
}

void UPlayerHealthComponent::ServerRPCDamageTaken_Implementation(const float DamageTaken)
{
	// Run only on server 
	if(!Player->HasAuthority())
		return; 
	
	MulticastRPCDamageTaken(DamageTaken); 
}

void UPlayerHealthComponent::MulticastRPCDamageTaken_Implementation(const float DamageTaken)
{
	Player->OnDamageTaken(DamageTaken); 
}

void UPlayerHealthComponent::IDied()
{
	Super::IDied();

	// Run only on the local player, place this before Super call? 
	if(!Player->IsLocallyControlled())
		return; 
	
	ServerRPCPlayerDied(); 
}

void UPlayerHealthComponent::ServerRPCPlayerDied_Implementation()
{
	// Run only on server 
	if(!Player->HasAuthority())
		return;

	MulticastRPCPlayerDied(); 
}

void UPlayerHealthComponent::MulticastRPCPlayerDied_Implementation()
{
	Player->OnPlayerDied(); 
}
