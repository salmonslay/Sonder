// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseHealthComponent.h"

#include "EnemyCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBaseHealthComponent::UBaseHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Note: Tick is turned off

	SetIsReplicatedByDefault(true); 

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
	
	if (GetOwner()->GetLocalRole() == ROLE_Authority) // körs på servern
	{
		// om authority - uppdatera hälsa
		DamageAmount = FMath::Min(GetHealth(), DamageAmount); 
		SetHealth(GetHealth() - DamageAmount);
		HealthUpdate();
	}

	UE_LOG(LogTemp, Warning, TEXT("Damage applied to %s: %f"), *GetOwner()->GetActorNameOrLabel(), DamageAmount)

	return DamageAmount; 
}

void UBaseHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseHealthComponent, CurrentHealth);
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

void UBaseHealthComponent::OnRep_HealthChange()
{
	HealthUpdate();
}

void UBaseHealthComponent::HealthUpdate()
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		if(IsDead())
			IDied();
	}
}

bool UBaseHealthComponent::IsDead() const
{
	return CurrentHealth <= 0;
}
