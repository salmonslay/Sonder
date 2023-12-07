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



void ASwapSpline::SplineSwap()
{
	
	if (CharactersCamera && NextCameraSpline)
	{
		ServerRPCSwap();
		
		
	}

	
}


void ASwapSpline::ServerRPCSwap_Implementation()
{
    if(!this->HasAuthority())
		return;
		
	MulticastRPCSwap();
}

void ASwapSpline::MulticastRPCSwap_Implementation()
{
	if (TransitionSpeed > 0)
	{
		CharactersCamera->SetInterpSpeed(TransitionSpeed);
	}
	else
	{
		CharactersCamera->SetInterpSpeed(CharactersCamera->DefaultInterpSpeed);
	}
	CharactersCamera->AssignSpline(NextCameraSpline);
}
