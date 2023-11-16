// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotBaseState.h"

#include "CollisionDebugDrawingPublic.h"
#include "EnhancedInputComponent.h"
#include "RobotHookingState.h"
#include "PulseObjectComponent.h"
#include "RobotStateMachine.h"
#include "SoulCharacter.h"
#include "Chaos/CollisionResolutionUtil.h"

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

	FTimerHandle PulseTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(PulseTimerHandle, this, &URobotBaseState::DisablePulseCooldown, PulseCooldown);

	// Code here is run only locally
	TArray<AActor*> OverlappingActors; 
	RobotCharacter->GetOverlappingActors(OverlappingActors, AActor::StaticClass()); // TODO: Replace the class filter with eventual better class (if it exists)

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
	
	// Code here is run on each player (client and server)
	TArray<AActor*> OverlappingActors; 
	RobotCharacter->GetOverlappingActors(OverlappingActors, AActor::StaticClass()); // TODO: Replace the class filter with eventual better class (if it exists)

	// calls take damage on every overlapping actor except itself
	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
	{
		if(Actor->ActorHasTag(FName("Soul")) && Actor->GetActorLocation().Z > RobotCharacter->GetActorLocation().Z + 5)
		{
			ASoulCharacter* Soul = Cast<ASoulCharacter>(Actor);
			PlayerActor = Cast<ACharacter>(Soul);
			UE_LOG(LogTemp, Warning, TEXT("Boost"));
			Soul->JumpMaxCount = 2;
			Soul->Jump();
			
			FTimerHandle MemberTimerHandle; 
			GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump, 1.0f, true, 2.0f); 
		}

		if(Actor->ActorHasTag(FName("Soul")) && Actor->GetActorLocation().Z + 20 < RobotCharacter->GetActorLocation().Z)
		{
			PlayerActor = Cast<ACharacter>(RobotCharacter);
			UE_LOG(LogTemp, Warning, TEXT("Boost"));
			RobotCharacter->JumpMaxCount = 2;
			RobotCharacter->Jump();
			
			FTimerHandle MemberTimerHandle; 
			GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump, 1.0f, true, 2.0f); 
		}
	}

	RobotCharacter->OnPulse(); 
}

