// Fill out your copyright notice in the Description page of Project Settings.


#include "FreezeConstraintWalls.h"

#include "CharactersCamera.h"
#include "PROJCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFreezeConstraintWalls::AFreezeConstraintWalls()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));
	TriggerArea->SetupAttachment(RootComponent);
	
	LeftWallLocation = CreateDefaultSubobject<USphereComponent>(TEXT("LeftLoc"));
	LeftWallLocation->SetupAttachment(TriggerArea);

	RightWallLocation = CreateDefaultSubobject<USphereComponent>(TEXT("RightLoc"));
	RightWallLocation->SetupAttachment(TriggerArea);
	
}



// Called when the game starts or when spawned
void AFreezeConstraintWalls::BeginPlay()
{
	Super::BeginPlay();
	
    CharactersCamera = Cast<ACharactersCamera>(UGameplayStatics::GetActorOfClass(this,ACharactersCamera::StaticClass()));

	TriggerArea->OnComponentBeginOverlap.AddDynamic(this,&AFreezeConstraintWalls::OverlapBegin);
	TriggerArea->OnComponentEndOverlap.AddDynamic(this,&AFreezeConstraintWalls::OverlapEnd);
	
	
}

void AFreezeConstraintWalls::MoveWallsToLoc()
{
	CharactersCamera->SetAllowWallsMovement(false);
	if (CharactersCamera->WallOne && CharactersCamera->WallTwo)
	{
		CharactersCamera->WallOne->SetActorLocation(LeftWallLocation->GetComponentLocation());
		CharactersCamera->WallTwo->SetActorLocation(RightWallLocation->GetComponentLocation());
	}
		
}

void AFreezeConstraintWalls::RemoveWallsFromLoc()
{
	CharactersCamera->SetAllowWallsMovement(true);
}





