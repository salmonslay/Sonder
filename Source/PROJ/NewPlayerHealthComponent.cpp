// Fill out your copyright notice in the Description page of Project Settings.


#include "NewPlayerHealthComponent.h"

#include "PROJCharacter.h"
#include "SoulBaseStateNew.h"


UNewPlayerHealthComponent::UNewPlayerHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Note: Tick is turned off 
}

float UNewPlayerHealthComponent::TakeDamage(float DamageAmount)
{
	const float DamageTaken = Super::TakeDamage(DamageAmount);

	// No actual damage taken, dont do anything 
	if(DamageTaken <= 0)
		return DamageTaken; 

	ServerRPCDamageTaken(DamageAmount); 

	return DamageTaken; 
}

void UNewPlayerHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APROJCharacter>(GetOwner());
	SoulState = Player->FindComponentByClass<USoulBaseStateNew>(); 
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

	ServerRPCPlayerDied(); 
}

void UNewPlayerHealthComponent::ServerRPCPlayerDied_Implementation()
{
	if(!Player->HasAuthority())
		return;
	
	MulticastRPCPlayerDied(); 
}

void UNewPlayerHealthComponent::MulticastRPCPlayerDied_Implementation()
{
	Player->OnPlayerDied();

	if(SoulState)
		SoulState->EndGrenadeThrowWithoutThrowing(); 
	
	//CurrentHealth = MaxHealth;
}
