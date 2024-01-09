// Fill out your copyright notice in the Description page of Project Settings.


#include "SonderGameState.h"

#include "PROJCharacter.h"
#include "PROJGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


EDifficulty ASonderGameState::GetDifficulty()
{
	return Difficulty;
}

void ASonderGameState::SetDifficulty(const EDifficulty DiffToSet)
{
	Difficulty = DiffToSet;
}

bool ASonderGameState::BothPlayersAreSet()
{
	return GetServerPlayer() && GetClientPlayer();
}

APROJCharacter* ASonderGameState::GetServerPlayer()
{
	APROJCharacter* TempCharacter = nullptr;

	if (PlayerArray.Num() >= 1)
	{
		TempCharacter = Cast<APROJCharacter>(PlayerArray[0]->GetPawn());
	}
	
	return TempCharacter;
}

APROJCharacter* ASonderGameState::GetClientPlayer()
{
	APROJCharacter* TempCharacter = nullptr;

	if (PlayerArray.Num() >= 2)
	{
		TempCharacter = Cast<APROJCharacter>(PlayerArray[1]->GetPawn());
	}
	
	return TempCharacter;
}

bool ASonderGameState::IsInCameraFieldOfView(const FVector& Location) const
{
	const APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	if (PlayerCam == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Player camera is nullptr"))
		return false;
	}
	
	// Get the camera loc and rot, calculate direction of cam
	const FVector CameraLocation = PlayerCam->GetCameraLocation();
	const FVector CameraForwardVector = PlayerCam->GetActorForwardVector();
	
	const FVector DirectionToLocation = (Location - CameraLocation).GetSafeNormal();

	const float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CameraForwardVector, DirectionToLocation)));

	// Check if the angle is within the field of view, set true if yes
	if (Angle <= PlayerCam->GetFOVAngle() / 2)
	{
		return true;
	}
	return false;
}
