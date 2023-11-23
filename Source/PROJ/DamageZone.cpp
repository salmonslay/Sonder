// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageZone.h"

#include "NewPlayerHealthComponent.h"
#include "PROJCharacter.h"
#include "Engine/DamageEvents.h"

ADamageZone::ADamageZone()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADamageZone::BeginPlay()
{
	Super::BeginPlay();
}

void ADamageZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority()) // Only run on server
		return;

	// A damage zone can only damage the local player. If the player is not locally controlled, we ignore them.
	if (OtherActor->ActorHasTag("Player"))
	{
		PlayerActors.AddUnique(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Added %s to %s"), *OtherActor->GetName(), *GetName())
	}
}

void ADamageZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!HasAuthority()) // Only run on server
		return;

	if (OtherActor->ActorHasTag("Player"))
	{
		PlayerActors.Remove(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Removed %s from %s"), *OtherActor->GetName(), *GetName())

		PlayersDamagedThisOverlap.Remove(OtherActor);
	}
}

void ADamageZone::Tick(float DeltaSeconds)
{
	// this func is impossible to read, but hey at least it's battle tested (SpaceLeek)
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || PlayerActors.IsEmpty())
		return;

	for (AActor* PlayerActor : PlayerActors)
	{
		APROJCharacter* Character = Cast<APROJCharacter>(PlayerActor);
		if (!Character)
			continue;

		if (GetWorld()->GetTimeSeconds() - LastDamageTime > TimeUntilFirstDamage)
		{
			// Handle instant kill
			if (bInstantKill)
			{
				if (!PlayersDamagedThisOverlap.Contains(PlayerActor))
				{
					PlayersDamagedThisOverlap.Add(PlayerActor);
					UE_LOG(LogTemp, Warning, TEXT("Killing %s from %s"), *PlayerActor->GetName(), *GetName())

					// Damage is not dealt if players cannot be damaged 
					// Character->TakeDamage(TNumericLimits<float>::Max(), FDamageEvent(), nullptr, this);

					// Calls IDied directly instead 
					if(const auto HealthComp = Character->FindComponentByClass<UNewPlayerHealthComponent>())
						HealthComp->IDied(); 
					
					PlayerActors.Remove(PlayerActor);
					PlayersDamagedThisOverlap.Remove(PlayerActor);
				}
				return;
			}

			// Early return if we've already been damaged and don't want to do it again
			if (bOnlyDamageOnce && PlayersDamagedThisOverlap.Contains(PlayerActor))
				return;

			UE_LOG(LogTemp, Warning, TEXT("Damaging %s from %s"), *PlayerActor->GetName(), *GetName())
			Character->TakeDamage(DamageAmount, FDamageEvent(), nullptr, this);

			LastDamageTime = GetWorld()->GetTimeSeconds();
			PlayersDamagedThisOverlap.Add(PlayerActor);
		}
	}
}
