// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraSpline.h"

#include "CharactersCamera.h"
#include "Components/BoxComponent.h"

// Sets default values
ACameraSpline::ACameraSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpline = CreateDefaultSubobject<USplineComponent>("CameraSpline");

	CameraRotation = CreateDefaultSubobject<UStaticMeshComponent>("CameraRotation");

	CameraRotation->SetupAttachment(CameraSpline);

	CameraEnterBox = CreateDefaultSubobject<UBoxComponent>("CameraEnterBox");

	CameraEnterBox->SetupAttachment(CameraSpline);
	

}

void ACameraSpline::BeginPlay()
{
	Super::BeginPlay();

	CameraEnterBox->OnComponentBeginOverlap.AddDynamic(this,&ACameraSpline::ActorBeginOverlap);
}

void ACameraSpline::ActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharactersCamera* Camera = Cast<ACharactersCamera>(OtherActor))
	{
		if (bIsCameraSpline)
		{
			Camera->ResetInterpSpeed();
		}
		
	}
}



