// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageZone.h"

#include "PROJCharacter.h"

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

		PlayersDamagedThisOverlap.Remove(OtherActor);
	}
}

void ADamageZone::Tick(float DeltaSeconds)
{
	// this func is impossible to read, but hey at least it's battle tested (SpaceLeek)
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || PlayerActors.IsEmpty())
		return;

	for (AActor* Player : PlayerActors)
	{
		APROJCharacter* PPlayer = Cast<APROJCharacter>(Player);
		if (GetWorld()->GetTimeSeconds() - LastDamageTime > TimeUntilFirstDamage)
		{
			// Handle instant kill
			if (bInstantKill)
			{
				if (!PlayersDamagedThisOverlap.Contains(Player))
				{
					PlayersDamagedThisOverlap.Add(Player);
					UE_LOG(LogTemp, Warning, TEXT("Killing %s from %s"), *Player->GetName(), *GetName())

					// Player->KillPlayer(); // TODO: implement real killing
					
					// teleport to its checkpoint TODO: remove this when we have real killing
					PPlayer->SetActorLocation(PPlayer->GetSpawnTransform().GetLocation());
					PPlayer->SetActorRotation(PPlayer->GetSpawnTransform().GetRotation());
				}
				return;
			}

			// Early return if we've already been damaged and don't want to do it again
			if (bOnlyDamageOnce && PlayersDamagedThisOverlap.Contains(Player))
				return;

			UE_LOG(LogTemp, Warning, TEXT("Damaging %s from %s"), *Player->GetName(), *GetName())
			// PlayerState->TakeDamage(DamageAmount, FDamageEvent(), nullptr, nullptr); // TODO: implement dealing damage
			LastDamageTime = GetWorld()->GetTimeSeconds();
			PlayersDamagedThisOverlap.Add(Player);
		}
	}
}
