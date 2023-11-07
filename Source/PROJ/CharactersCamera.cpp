// Fill out your copyright notice in the Description page of Project Settings.


#include "CharactersCamera.h"

#include "PROJCharacter.h"
#include "PROJGameMode.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

ACharactersCamera::ACharactersCamera()
{
	CameraComponent = this->GetCameraComponent();
	PrimaryActorTick.bCanEverTick = true;
}

void ACharactersCamera::BeginPlay()
{
	Super::BeginPlay();
	if (CameraSplineClass)
	{
		CameraSpline = CameraSplineClass->CameraSpline;
	}
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,this, &ACharactersCamera::GetPlayers,2.0f);
	
}

void ACharactersCamera::GetPlayers()
{
	if (APROJGameMode* CurrentGameMode = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(this)); CurrentGameMode != nullptr)
	{
		PlayerOne = CurrentGameMode->GetActivePlayer(0);
		if (const AGameStateBase* Gsb = UGameplayStatics::GetGameState(this); Gsb->PlayerArray.Num() > 1)
		{
			PlayerTwo = CurrentGameMode->GetActivePlayer(1);
		}
		
		bAllowMovement = true;
	}
}

void ACharactersCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	MoveCamera();
	
}

void ACharactersCamera::MoveCamera() const
{
	if (bAllowMovement)
	{
		if (CameraSpline)
		{
			
		
			if (PlayerOne != nullptr && PlayerTwo != nullptr)
			{
				const FVector MiddleLocation = (PlayerOne->GetActorLocation()/2) + (PlayerTwo->GetActorLocation()/2);
				const FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(MiddleLocation, ESplineCoordinateSpace::World);
				const FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeed);
					CameraComponent->SetWorldLocation(TargetLocation);
				
			} else if (PlayerTwo == nullptr)
			{
				const FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerOne->GetActorLocation(), ESplineCoordinateSpace::World);
				const FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeed);
				CameraComponent->SetWorldLocation(TargetLocation);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No Players"));
			}
		}
	}
		
		/*
		else if (PlayerTwo == nullptr)
		{
			FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerOne->GetActorLocation(), ESplineCoordinateSpace::World);
			FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeed);
			CameraComponent->SetWorldLocation(TargetLocation);
		}
		else if (PlayerOne == nullptr)
		{
			FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerTwo->GetActorLocation(), ESplineCoordinateSpace::World);
			FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeed);
			CameraComponent->SetWorldLocation(TargetLocation);
		}
		else
		{
			
		}*/
		
		
	
	
}





