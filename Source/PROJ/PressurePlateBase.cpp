// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlateBase.h"

// Sets default values
APressurePlateBase::APressurePlateBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

