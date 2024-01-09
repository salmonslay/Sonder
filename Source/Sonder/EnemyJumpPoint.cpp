// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyJumpPoint.h"

#include "Components/BoxComponent.h"

// Sets default values
AEnemyJumpPoint::AEnemyJumpPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Point = CreateDefaultSubobject<UBoxComponent>(TEXT("Jump point"));
	Point->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemyJumpPoint::BeginPlay()
{
	Super::BeginPlay();

	Location = Point->GetComponentLocation();
	
}

// Called every frame
void AEnemyJumpPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

