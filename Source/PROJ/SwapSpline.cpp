// Fill out your copyright notice in the Description page of Project Settings.


#include "SwapSpline.h"

#include "CharactersCamera.h"
#include "PROJCharacter.h"

// Sets default values
ASwapSpline::ASwapSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));

	TriggerZone->SetupAttachment(RootComponent);
	

}

// Called when the game starts or when spawned
void ASwapSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASwapSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}



void ASwapSpline::SplineSwap()
{
	if (CharactersCamera && NextCameraSpline)
	{
		CharactersCamera->AssignSpline(NextCameraSpline);
		
	}
}

