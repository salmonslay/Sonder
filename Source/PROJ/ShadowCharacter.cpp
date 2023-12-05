// Fill out your copyright notice in the Description page of Project Settings.

#include "ShadowCharacter.h"

#include "DummyPlayerState.h"
#include "EnemyBasicAttack.h"
#include "PlayerCharState.h"
#include "RobotBaseState.h"
#include "SoulBaseStateNew.h"
#include "Net/UnrealNetwork.h"

AShadowCharacter::AShadowCharacter()
{
	// Create the states 
	DummyState = CreateDefaultSubobject<UDummyPlayerState>(TEXT("DummyStateNew"));

	EnemyBasicAttack = CreateDefaultSubobject<UEnemyBasicAttack>(TEXT("BasicAttack"));
	EnemyBasicAttack->SetupAttachment(RootComponent);

}

void AShadowCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShadowCharacter, CurrentState) 
}

void AShadowCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = GetStartingState();

	if(CurrentState)
		CurrentState->Enter();
}

void AShadowCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!IsLocallyControlled())
		return; 

	if(CurrentState)
		CurrentState->Update(DeltaSeconds);
}

UPlayerCharState* AShadowCharacter::GetStartingState() const
{
	if(const auto SoulBaseState = FindComponentByClass<USoulBaseStateNew>())
		return SoulBaseState;
	
	if(const auto RobotBaseState = FindComponentByClass<URobotBaseState>())
		return RobotBaseState;

	UE_LOG(LogTemp, Warning, TEXT("Error. %s has no base state"), *GetActorNameOrLabel())
	return DummyState; // Should not get here 
}

void AShadowCharacter::ServerRPC_SwitchState_Implementation(UPlayerCharState* NewState)
{
	if(!HasAuthority())
		return;

	CurrentState = NewState; 
}

void AShadowCharacter::SwitchState(UPlayerCharState* NewState)
{
	if(CurrentState == NewState)
		return; 
	
	if(CurrentState)
		CurrentState->Exit();

	ServerRPC_SwitchState(NewState); 
	CurrentState = NewState;

	CurrentState->Enter();
}
