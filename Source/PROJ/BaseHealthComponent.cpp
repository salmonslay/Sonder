// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHealthComponent.h"

#include "EnemyCharacter.h"


// Sets default values for this component's properties
UBaseHealthComponent::UBaseHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBaseHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = MaxHealth;

}


// Called every frame
void UBaseHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


float UBaseHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

float UBaseHealthComponent::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

void UBaseHealthComponent::SetHealth(float NewHealth)
{
	CurrentHealth = FMath::Min(MaxHealth, FMath::Max(0, NewHealth));
}

void UBaseHealthComponent::SetMaxHealth(float NewHealth)
{
	MaxHealth = NewHealth;
}

void UBaseHealthComponent::RefillHealth()
{
	CurrentHealth = MaxHealth;
}

bool UBaseHealthComponent::IsDead() const
{
	return CurrentHealth <= 0;
}