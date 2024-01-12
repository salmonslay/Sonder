// Fill out your copyright notice in the Description page of Project Settings.


#include "CharactersCamera.h"

#include "PROJCharacter.h"
#include "PROJGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACharactersCamera::ACharactersCamera()
{
	CameraComp = GetCameraComponent();
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
}

void ACharactersCamera::BeginPlay()
{
	Super::BeginPlay();

	AssignSpline(DefaultCameraSplineClass);
	
	DefaultInterpSpeed = InterpSpeedLocation;
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ACharactersCamera::GetPlayers, 2.0f);
	
	if (WallOne && WallTwo)
	{
		WallOne->SetActorEnableCollision(false);
		WallTwo->SetActorEnableCollision(false);
	}

	FTimerHandle WallHandle;
	GetWorld()->GetTimerManager().SetTimer(WallHandle, this, &ACharactersCamera::SetWallCollision, 3.0f);
}

ACameraSpline* ACharactersCamera::AssignSpline(ACameraSpline* CameraSplineClass)
{
	if (CameraSplineClass)
	{
		CameraSpline = CameraSplineClass->CameraSpline;
		CurrentCameraSplineClass = CameraSplineClass;

		return CurrentCameraSplineClass;
	}
	return nullptr;
}

void ACharactersCamera::GetPlayers()
{
	TArray<AActor*> Players; 
	UGameplayStatics::GetAllActorsOfClass(this, APROJCharacter::StaticClass(), Players);

	if(!Players.IsEmpty())
	{
		PlayerOne = Cast<APROJCharacter>(Players[0]);

		if(Players.Num() > 1)
			PlayerTwo = Cast<APROJCharacter>(Players[1]);

		bAllowMovement = true; 
	}
	
	
}

void ACharactersCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveCamera();

	RotateCamera();
}

void ACharactersCamera::RotateCamera() const
{
	if (CameraSpline)
	{
		const FRotator Rotate = CurrentCameraSplineClass->CameraRotation->GetComponentRotation();
		const FRotator RotateCorrect = FMath::RInterpTo(CameraComp->GetComponentRotation(), Rotate, FApp::GetDeltaTime(), InterpSpeedRotation);
		if (CameraComp->GetComponentRotation() != Rotate)
		{
			CameraComp->SetWorldRotation(RotateCorrect);
		}
	}
}

void ACharactersCamera::MoveWalls(const FVector MiddlePoint) const
{
	if(!HasAuthority())
		return; 
	
	if (WallOne && WallTwo && bAllowWallsMovement)
	{
		const float FOV = CameraComp->FieldOfView / 2;
		const FVector Camloc = CameraComp->GetComponentLocation();
		const double Offset = FMath::Tan(FOV) * (FVector::Distance(MiddlePoint, Camloc));

		WallOne->SetActorLocation(FVector(MiddlePoint.X, (MiddlePoint.Y - (Offset * 1.5) / 2), MiddlePoint.Z));
		WallTwo->SetActorLocation(FVector(MiddlePoint.X, MiddlePoint.Y + (Offset * 1.5) / 2, MiddlePoint.Z));
	}
}

void ACharactersCamera::SetWallCollision() const
{
	if (WallOne && WallTwo)
	{
		WallOne->SetActorEnableCollision(true);
		WallTwo->SetActorEnableCollision(true);
	}
}

void ACharactersCamera::MoveCamera()
{
	if (bAllowMovement)
	{
		if (CameraSpline)
		{
			if (PlayerOne != nullptr && PlayerTwo != nullptr) // if both players are in the game
			{
				const FVector MiddleLocation = PlayerOne->GetActorLocation() / 2 + PlayerTwo->GetActorLocation() / 2;
				MoveWalls(MiddleLocation);
				const FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(MiddleLocation, ESplineCoordinateSpace::World);
				TargetLocation = FMath::VInterpTo(CameraComp->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeedLocation);
				CameraComp->SetWorldLocation(TargetLocation);
				
			}
			else if (PlayerTwo == nullptr && PlayerOne != nullptr) // if only player one is in the game 
			{
				const FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerOne->GetActorLocation(), ESplineCoordinateSpace::World);
				//MoveWalls(ActorLocations);
				TargetLocation = FMath::VInterpTo(CameraComp->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeedLocation);
				CameraComp->SetWorldLocation(TargetLocation);
				
			}
			else if (PlayerOne == nullptr && PlayerTwo != nullptr) // if only player two is in the game
			{
				const FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerTwo->GetActorLocation(), ESplineCoordinateSpace::World);
				//MoveWalls(ActorLocations);
				TargetLocation = FMath::VInterpTo(CameraComp->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeedLocation);
				CameraComp->SetWorldLocation(TargetLocation);
			
			}
			else // if no players are in the game
			{
				UE_LOG(LogTemp, Warning, TEXT("No players in-game"))
			}
		}
	}
}


void ACharactersCamera::SetInterpSpeed(double SwapInterpSpeed)
{
	InterpSpeedLocation = SwapInterpSpeed;
}

void ACharactersCamera::ResetInterpSpeed()
{
	InterpSpeedLocation = DefaultInterpSpeed;
}

bool ACharactersCamera::SetAllowWallsMovement(bool bValue){

	return bAllowWallsMovement = bValue;
	
}
