// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyPlayerState.h"

void UDummyPlayerState::Enter()
{
	Super::Enter();

	UE_LOG(LogTemp, Warning, TEXT("Entered dummy state, actor: %s"), *GetOwner()->GetActorNameOrLabel())
}

void UDummyPlayerState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	// UE_LOG(LogTemp, Warning, TEXT("Updating dummy state"))
}

void UDummyPlayerState::Exit()
{
	Super::Exit();

	UE_LOG(LogTemp, Warning, TEXT("Exiting dummy state"))
}
