// Fill out your copyright notice in the Description page of Project Settings.


#include "SwapSpline.h"

#include "CharactersCamera.h"
#include "PROJCharacter.h"

// Sets default values
ASwapSpline::ASwapSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));

	TriggerArea->SetupAttachment(RootComponent);
	

}

// Called when the game starts or when spawned
void ASwapSpline::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Begin"));
	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &ASwapSpline::OverlapBegin);
}

// Called every frame
void ASwapSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

void ASwapSpline::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor->ActorHasTag("Player"))
	{
		SplineSwap();
		UE_LOG(LogTemp, Warning, TEXT("Player Overlaps"));
	}
}

void ASwapSpline::SplineSwap()
{
	if (CharactersCamera && NextCameraSpline)
	{
		CharactersCamera->AssignSpline(NextCameraSpline);
		
	}
}

