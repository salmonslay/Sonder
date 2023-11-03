// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjPlayerController.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

void AProjPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetCamera(); 
}

void AProjPlayerController::SetCamera()
{
	const auto MainCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));

	if(IsValid(MainCamera)) 
		SetViewTarget(MainCamera);
}
