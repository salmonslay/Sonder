// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageZone.h"

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

	// A damage zone can only damage the local player. If the player is not locally controlled, we ignore them.
	if (OtherActor->ActorHasTag("Player") && OtherActor->GetLocalRole() == ROLE_Authority)
	{
		PlayerActor = OtherActor;
		bIsPlayerInZone = true;
	}
}

void ADamageZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (OtherActor->ActorHasTag("Player"))
	{
		PlayerActor = nullptr;
		bIsPlayerInZone = false;

		bHasBeenDamagedDuringThisOverlap = false;
		bHasPlayedEffectsDuringThisOverlap = false;
	}
}

void ADamageZone::Tick(float DeltaSeconds)
{
	// this func is impossible to read, but hey at least it's battle tested (SpaceLeek)
	Super::Tick(DeltaSeconds);

	if (!bIsPlayerInZone)
		return;

	if (GetWorld()->GetTimeSeconds() - LastDamageTime > TimeUntilFirstDamage)
	{
		// Handle instant kill
		if (bInstantKill)
		{
			if (!bHasBeenDamagedDuringThisOverlap)
			{
				bHasBeenDamagedDuringThisOverlap = true;
				UE_LOG(LogTemp, Warning, TEXT("Killing player from %s"), *GetName())

				if (!bHasPlayedEffectsDuringThisOverlap)
					PlayEffects();

				// PlayerState->KillPlayer(); // TODO: implement killing
			}
			return;
		}

		// Early return if we've already been damaged and don't want to do it again
		if (bOnlyDamageOnce && bHasBeenDamagedDuringThisOverlap)
			return;

		UE_LOG(LogTemp, Warning, TEXT("Damaging player from %s"), *GetName())
		// PlayerState->TakeDamage(DamageAmount, FDamageEvent(), nullptr, nullptr); // TODO: implement dealing damage
		LastDamageTime = GetWorld()->GetTimeSeconds();
		bHasBeenDamagedDuringThisOverlap = true;

		if (DamageAmount > 10 && !bHasPlayedEffectsDuringThisOverlap)
			PlayEffects();
	}
}


void ADamageZone::PlayEffects()
{
	// TODO: We used this func for water in SpaceLeek. Not sure if it will be useful here.

	/*UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SplashEffect,
	                                               PlayerActor->GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SplashSound, PlayerActor->GetActorLocation());*/
	bHasPlayedEffectsDuringThisOverlap = true;
}
