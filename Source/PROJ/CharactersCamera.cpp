// Fill out your copyright notice in the Description page of Project Settings.


#include "CharactersCamera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ACharactersCamera::ACharactersCamera()
{
	CameraComponent = this->GetCameraComponent();
	
}

void ACharactersCamera::BeginPlay()
{
	CameraSpline = CameraSplineClass->CameraSpline;
	PlayerOne = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	PlayerTwo = UGameplayStatics::GetPlayerCharacter(GetWorld(),1);
}


void ACharactersCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveCamera();
	
}

void ACharactersCamera::MoveCamera()
{
	FVector ActorLocations = CameraSpline->FindLocationClosestToWorldLocation(PlayerOne->GetActorLocation(), ESplineCoordinateSpace::World);
	FVector TargetLocation = FMath::VInterpTo(CameraComponent->GetComponentLocation(), ActorLocations, FApp::GetDeltaTime(), 0.5f);
	this->SetActorLocation(TargetLocation);
}



