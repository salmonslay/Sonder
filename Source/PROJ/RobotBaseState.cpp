// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotBaseState.h"

#include "EnhancedInputComponent.h"
#include "RobotHookingState.h"
#include "PulseObjectComponent.h"
#include "RobotStateMachine.h"

void URobotBaseState::Enter()
{
	Super::Enter();

	if(!RobotCharacter)
		RobotCharacter = Cast<ARobotStateMachine>(PlayerOwner);
}

void URobotBaseState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	
}

void URobotBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	// UE_LOG(LogTemp, Warning, TEXT("Setting up input, lcl ctrl: %i"), PlayerOwner->IsLocallyControlled())

	if(!bHasSetUpInput)
	{
		InputComp->BindAction(HookShotInputAction, ETriggerEvent::Started, this, &URobotBaseState::ShootHook);

		InputComp->BindAction(PulseInputAction, ETriggerEvent::Started, this, &URobotBaseState::Pulse);
		
		bHasSetUpInput = true; 
	}
}

void URobotBaseState::Exit()
{
	Super::Exit();

	
}

void URobotBaseState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void URobotBaseState::ShootHook()
{
	if(bHookShotOnCooldown)
		return; 
	
	// UE_LOG(LogTemp, Warning, TEXT("Fired hook"))

	bHookShotOnCooldown = true;
	
	// Enable hook shot after set time 
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &URobotBaseState::DisableHookShotCooldown, HookShotCooldownDelay); 
	
	PlayerOwner->SwitchState(RobotCharacter->HookState);
}

void URobotBaseState::Pulse()
{
	// Ensure player cant spam attack and is locally controlled 
	// Only run locally 
	if(bPulseCoolDownActive || !PlayerOwner->IsLocallyControlled())
		return;

	bPulseCoolDownActive = true;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &URobotBaseState::DisablePulseCooldown, 1);

	// Code here is run only locally
	TArray<AActor*> OverlappingActors; 
	RobotCharacter->GetOverlappingActors(OverlappingActors, AActor::StaticClass()); // TODO: Replace the class filter with eventual better class (if it exists)

	// calls take damage on every overlapping actor except itself
	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
	{
		// Friendly fire off 
		//if(!Actor->ActorHasTag(FName("PulseObject")))
		//	Cast<UPulseObjectComponent>(Actor)->OnActivate();
	}

	bPulseCoolDownActive = false;

	ServerRPCPulse();
}

void URobotBaseState::ServerRPCPulse_Implementation()
{
	// Code here is only run on server, will probably not be changed unless we'll have server specific behaviour 
	
	// Should only run on server 
	if(!GetOwner()->HasAuthority())
		return; 

	// UE_LOG(LogTemp, Warning, TEXT("Server pulse"))
	
	MulticastRPCPulse();
}

void URobotBaseState::MulticastRPCPulse_Implementation()
{
	
	// UE_LOG(LogTemp, Warning, TEXT("Multicast attack"))

	RobotCharacter->OnPulse(); 
}

