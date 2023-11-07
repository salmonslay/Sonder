// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlateBase.h"

#include "Components/BoxComponent.h"

// Sets default values
APressurePlateBase::APressurePlateBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));

	TriggerArea->SetupAttachment(RootComponent);

	PressurePlateMesh =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));

	PressurePlateMesh->SetupAttachment(TriggerArea);	

}

// Called when the game starts or when spawned
void APressurePlateBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APressurePlateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

