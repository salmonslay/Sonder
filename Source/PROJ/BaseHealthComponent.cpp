// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHealthComponent.h"

#include "EnemyCharacter.h"

// Sets default values for this component's properties
UBaseHealthComponent::UBaseHealthComponent()
{
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
		IDied();
	
	UE_LOG(LogTemp, Warning, TEXT("Damage applied to %s: %f"), *GetOwner()->GetActorNameOrLabel(), DamageAmount)

	return DamageAmount; 
}

void UBaseHealthComponent::IDied()
{
	// TODO: Everything that should happen when the owner dies 
	UE_LOG(LogTemp, Warning, TEXT("%s died"), *GetOwner()->GetActorNameOrLabel()) 
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