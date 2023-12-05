// Fill out your copyright notice in the Description page of Project Settings.


#include "FreezeConstraintWalls.h"

#include "CharactersCamera.h"
#include "PROJCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AFreezeConstraintWalls::AFreezeConstraintWalls()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	LeftWallLocation = CreateDefaultSubobject<USphereComponent>(TEXT("LeftLoc"));
	LeftWallLocation->SetupAttachment(RootComponent);
	
	RightWallLocation = CreateDefaultSubobject<USphereComponent>(TEXT("RightLoc"));
	RightWallLocation->SetupAttachment(RootComponent);
}



// Called when the game starts or when spawned
void AFreezeConstraintWalls::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFreezeConstraintWalls::MoveWallsToLoc()
{
	CharactersCamera->SetAllowWallsMovement(false);
	CharactersCamera->WallOne->SetActorLocation(LeftWallLocation->GetComponentLocation());
	CharactersCamera->WallTwo->SetActorLocation(RightWallLocation->GetComponentLocation());
}

void AFreezeConstraintWalls::RemoveWallsFromLoc()
{
	CharactersCamera->SetAllowWallsMovement(true);
}



