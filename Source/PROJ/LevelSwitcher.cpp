// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSwitcher.h"

#include "SonderGameInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void ALevelSwitcher::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// only the server should do this
	if (!HasAuthority())
		return;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		OverlappingActors.Add(Character);

		if (OverlappingActors.Num() >= 2 && !bIsSwitching)
		{
			bIsSwitching = true;

			UE_LOG(LogTemp, Display, TEXT("Changing level to %s"), *UEnum::GetValueAsString(LevelToLoad));
			Cast<USonderGameInstance>(GetGameInstance())->LoadNewLevel(LevelToLoad);
		}
	}
}

void ALevelSwitcher::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!HasAuthority())
		return;

	OverlappingActors.Remove(OtherActor);
}
