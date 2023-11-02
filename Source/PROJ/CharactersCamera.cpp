// Fill out your copyright notice in the Description page of Project Settings.


#include "CharactersCamera.h"

#include "Camera/CameraComponent.h"

ACharactersCamera::ACharactersCamera()
{
	CameraComponent = this->GetCameraComponent();
}

void ACharactersCamera::BeginPlay()
{
	
}


void ACharactersCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CameraComponent->SetWorldLocation();
}



