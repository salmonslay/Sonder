// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHealthComponent.h"

#include "EnemyCharacter.h"

// Sets default values for this component's properties
UBaseHealthComponent::UBaseHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false; // Note: Tick is turned off 

}

// Called when the game starts
void UBaseHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

float UBaseHealthComponent::TakeDamage(float DamageAmount)
{
	if(IsDead())
		return 0; 
	
	DamageAmount = FMath::Min(GetHealth(), DamageAmount); 
	SetHealth(GetHealth() - DamageAmount);

	if(IsDead())
		PlayerDied();
	
	UE_LOG(LogTemp, Warning, TEXT("Damage applied to player: %f"), DamageAmount)

	return DamageAmount; 
}

void UBaseHealthComponent::PlayerDied()
{
	// TODO: Everything that should happen when a player dies 
	UE_LOG(LogTemp, Warning, TEXT("Player died")) 
}

float UBaseHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

float UBaseHealthComponent::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

void UBaseHealthComponent::SetHealth(const float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0, MaxHealth); 
}

void UBaseHealthComponent::SetMaxHealth(const float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}

void UBaseHealthComponent::RefillHealth()
{
	CurrentHealth = MaxHealth;
}

bool UBaseHealthComponent::IsDead() const
{
	return CurrentHealth <= 0;
}