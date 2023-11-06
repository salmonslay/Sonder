// Fill out your copyright notice in the Description page of Project Settings.


#include "CharactersCamera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
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
	PlayerOne = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	//PlayerTwo = UGameplayStatics::GetPlayerCharacter(GetWorld(),1);
	AGameStateBase::GetPlayerStateFromUniqueNetId()
}


void ACharactersCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	MoveCamera();
	
}

void ACharactersCamera::MoveCamera()
{
	if (PlayerOne && PlayerTwo)
	{
		FVector MiddlePostion = (PlayerOne->GetActorLocation()/2) + (PlayerTwo->GetActorLocation()/2);
		FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(MiddlePostion, ESplineCoordinateSpace::World);
		FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), InterpSpeed);
		CameraComponent->SetWorldLocation(TargetLocation);
	}
	
}



