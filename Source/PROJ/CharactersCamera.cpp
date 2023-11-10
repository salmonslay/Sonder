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
	CameraComponent = this->GetCameraComponent();
	PrimaryActorTick.bCanEverTick = true;
	
	
	bReplicates = true;

	
}

void ACharactersCamera::BeginPlay()
{
	Super::BeginPlay();
	
	AssignSpline(DefaultCameraSplineClass);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,this, &ACharactersCamera::GetPlayers,4.0f);


	if (WallOne && WallTwo)
	{
		WallOne->SetActorEnableCollision(false);
		WallTwo->SetActorEnableCollision(false);
	}
	
	FTimerHandle WallHandle;
	GetWorld()->GetTimerManager().SetTimer(WallHandle,this, &ACharactersCamera::SetWallCollision,5.0f);
	
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

	RotateCamera();
	
}

void ACharactersCamera::RotateCamera()
{
	if (CameraSpline)
	{
		
		FRotator Rotate = CurrentCameraSplineClass->CameraRotation->GetComponentRotation();
		FRotator RotateCorrect = FMath::RInterpTo(CameraComponent->GetComponentRotation(), Rotate,FApp::GetDeltaTime(),InterpSpeedRotation);
		if (CameraComponent->GetComponentRotation() != Rotate)
		{
			CameraComponent->SetWorldRotation(RotateCorrect);
		}
		
		
		
	}
	
}

void ACharactersCamera::MoveWalls(FVector MiddlePoint)
{
	if (WallOne && WallTwo)
	{
		float FOV = CameraComponent->FieldOfView/2;
		FVector Camloc = CameraComponent->GetComponentLocation();
		double Offset = FMath::Tan(FOV) * (FVector::Distance(MiddlePoint,Camloc));

		WallOne->SetActorLocation(FVector(MiddlePoint.X,(MiddlePoint.Y-Offset/2),MiddlePoint.Z));
		WallTwo->SetActorLocation(FVector(MiddlePoint.X,MiddlePoint.Y+Offset/2,MiddlePoint.Z));
	}
	
}

void ACharactersCamera::SetWallCollision()
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
			
			if (PlayerOne != nullptr && PlayerTwo != nullptr)
			{
				const FVector MiddleLocation = PlayerOne->GetActorLocation()/2 + PlayerTwo->GetActorLocation()/2;
				MoveWalls(MiddleLocation);
				const FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(MiddleLocation, ESplineCoordinateSpace::World);
				TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeedLocation);
				CameraComponent->SetWorldLocation(TargetLocation);
				
				
			} else if (PlayerTwo == nullptr)
			{
				const FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerOne->GetActorLocation(), ESplineCoordinateSpace::World);
				
				TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(),
				                                  InterpSpeedLocation);
				CameraComponent->SetWorldLocation(TargetLocation);
			}
			else if (PlayerOne == nullptr)
			{
				FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerTwo->GetActorLocation(), ESplineCoordinateSpace::World);
				TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeedLocation);
				CameraComponent->SetWorldLocation(TargetLocation);
				
			}
			else
			{
				
			}
		}
	}
		
		
	
	
}





