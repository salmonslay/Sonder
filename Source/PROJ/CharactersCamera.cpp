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
	CameraSpline = CameraSplineClass->CameraSpline;
	//PlayerOne = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	//PlayerTwo = UGameplayStatics::GetPlayerCharacter(GetWorld(),1);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,this, &ACharactersCamera::GetPlayers,2.0f);
	
}

void ACharactersCamera::GetPlayers()
{
	APROJGameMode* CurrentGameMode = Cast<APROJGameMode>(UGameplayStatics::GetGameMode(this));
	if (CurrentGameMode != nullptr)
	{
		PlayerOne = CurrentGameMode->GetActivePlayer(0);
		AGameStateBase* GSB = UGameplayStatics::GetGameState(this);
		if (GSB->PlayerArray.Num() > 1)
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

void ACharactersCamera::MoveCamera()
{
	if (bAllowMovement)
	{
		if (PlayerOne != nullptr && PlayerTwo != nullptr)
		{
				FVector MiddlePostion = (PlayerOne->GetActorLocation()/2) + (PlayerTwo->GetActorLocation()/2);
				FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(MiddlePostion, ESplineCoordinateSpace::World);
				FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeed);
				CameraComponent->SetWorldLocation(TargetLocation);
			
		} else if (PlayerTwo == nullptr)
		{
			FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerOne->GetActorLocation(), ESplineCoordinateSpace::World);
			FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeed);
			CameraComponent->SetWorldLocation(TargetLocation);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Players"));
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





