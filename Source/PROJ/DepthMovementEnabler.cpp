// Fill out your copyright notice in the Description page of Project Settings.


#include "DepthMovementEnabler.h"

#include "PROJCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ADepthMovementEnabler::ADepthMovementEnabler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box")); 
	TriggerBox->SetupAttachment(RootComponent);

	TriggerBox->SetBoxExtent(FVector::OneVector * 100); 
}

// Called when the game starts or when spawned
void ADepthMovementEnabler::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADepthMovementEnabler::OnComponentBeginOverlapped); 
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADepthMovementEnabler::OnComponentStopOverlap); 
}

void ADepthMovementEnabler::OnComponentBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(const auto Player = Cast<APROJCharacter>(OtherActor))
		Player->SetDepthMovementEnabled(true); 
}

void ADepthMovementEnabler::OnComponentStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int OtherBodyIndex)
{
	if(const auto Player = Cast<APROJCharacter>(OtherActor))
		Player->SetDepthMovementEnabled(false); 
}
