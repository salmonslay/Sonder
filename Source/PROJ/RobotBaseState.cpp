// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotBaseState.h"

#include "CollisionDebugDrawingPublic.h"
#include "EnemyCharacter.h"
#include "EnhancedInputComponent.h"
#include "LightGrenade.h"
#include "RobotHookingState.h"
#include "PulseObjectComponent.h"
#include "RobotStateMachine.h"
#include "SoulCharacter.h"
#include "Chaos/CollisionResolutionUtil.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

URobotBaseState::URobotBaseState()
{
	SetIsReplicatedByDefault(true);
}

void URobotBaseState::Enter()
{
	Super::Enter();

	if (!RobotCharacter)
		RobotCharacter = Cast<ARobotStateMachine>(PlayerOwner);

	DefaultWalkSpeed = PlayerOwner->GetCharacterMovement()->MaxWalkSpeed;
}

void URobotBaseState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);
}

void URobotBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	// UE_LOG(LogTemp, Warning, TEXT("Setting up input, lcl ctrl: %i"), PlayerOwner->IsLocallyControlled())

	if (!bHasSetUpInput)
	{
		InputComp->BindAction(HookShotInputAction, ETriggerEvent::Started, this, &URobotBaseState::ShootHook);

		InputComp->BindAction(PulseInputAction, ETriggerEvent::Started, this, &URobotBaseState::Pulse);

		InputComp->BindAction(AbilityInputAction, ETriggerEvent::Started, this, &URobotBaseState::ActivateAbilities);

		bHasSetUpInput = true;
	}
}

void URobotBaseState::Exit()
{
	Super::Exit();
}

void URobotBaseState::ApplySoulDashBuff()
{
	// Clear timer so it resets if there already is an existing buff, then start it again 
	GetWorld()->GetTimerManager().ClearTimer(BuffTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &URobotBaseState::ResetDashBuff, DashBuffLength);

	// TODO: idk if we want to fire event again if buffed while already buffed. the rest does not need to update 
	if (!bHasDashBuff)
		ServerRPC_DashBuffStart();
}

void URobotBaseState::ServerRPC_DashBuffStart_Implementation()
{
	if (!PlayerOwner->HasAuthority())
		return;

	bHasDashBuff = true;

	MulticastRPC_DashBuffStart();
}

void URobotBaseState::MulticastRPC_DashBuffStart_Implementation()
{
	PlayerOwner->GetCharacterMovement()->MaxWalkSpeed = WalkSpeedWhenBuffed;

	RobotCharacter->OnDashBuffStart();
}

void URobotBaseState::ResetDashBuff()
{
	ServerRPC_DashBuffEnd();
}

void URobotBaseState::ServerRPC_DashBuffEnd_Implementation()
{
	if (!PlayerOwner->HasAuthority())
		return;

	bHasDashBuff = false;

	MulticastRPC_DashBuffEnd();
}

void URobotBaseState::MulticastRPC_DashBuffEnd_Implementation()
{
	PlayerOwner->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	RobotCharacter->OnDashBuffEnd();
}

void URobotBaseState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void URobotBaseState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URobotBaseState, bHasDashBuff)
}

void URobotBaseState::ShootHook()
{
	if (bHookShotOnCooldown || !RobotCharacter->AbilityTwo)
		return;

	// UE_LOG(LogTemp, Warning, TEXT("Fired hook"))

	bHookShotOnCooldown = true;

	// Enable hook shot after set time 
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &URobotBaseState::DisableHookShotCooldown,
	                                       HookShotCooldownDelay);

	PlayerOwner->SwitchState(RobotCharacter->HookState);
}

void URobotBaseState::Pulse()
{
	// Ensure player cant spam attack and is locally controlled 
	// Only run locally 
	if (bPulseCoolDownActive || !PlayerOwner->IsLocallyControlled() || !RobotCharacter->AbilityOne)
		return;

	bPulseCoolDownActive = true;

	FTimerHandle PulseTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(PulseTimerHandle, this, &URobotBaseState::DisablePulseCooldown,
	                                       PulseCooldown);

	// Code here is run only locally
	TArray<AActor*> OverlappingActors;
	RobotCharacter->GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	// TODO: Replace the class filter with eventual better class (if it exists)

	ServerRPCPulse();
}

void URobotBaseState::ServerRPCPulse_Implementation()
{
	// Code here is only run on server, will probably not be changed unless we'll have server specific behaviour 

	// Should only run on server 
	if (!GetOwner()->HasAuthority())
		return;

	// UE_LOG(LogTemp, Warning, TEXT("Server pulse"))

	MulticastRPCPulse();
}

void URobotBaseState::MulticastRPCPulse_Implementation()
{
	// Code here is run on each player (client and server)
	TArray<AActor*> OverlappingActors;
	RobotCharacter->GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	// TODO: Replace the class filter with eventual better class (if it exists)

	// calls take damage on every overlapping actor except itself
	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for (const auto Actor : OverlappingActors)
	{
		if (Actor->ActorHasTag(FName("Soul")) && Actor->GetActorLocation().Z > RobotCharacter->GetActorLocation().Z + 5)
		{
			ASoulCharacter* Soul = Cast<ASoulCharacter>(Actor);
			PlayerActor = Cast<ACharacter>(Soul);
			UE_LOG(LogTemp, Warning, TEXT("Boost"));
			Soul->GetCharacterMovement()->Velocity.Z = 0;
			Soul->JumpMaxCount = 2;
			Soul->Jump();

			FTimerHandle MemberTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump, 1.0f,
			                                       false);
		}

		if (Actor->ActorHasTag(FName("Soul")) && Actor->GetActorLocation().Z + 20 < RobotCharacter->GetActorLocation().
			Z)
		{
			PlayerActor = Cast<ACharacter>(RobotCharacter);
			UE_LOG(LogTemp, Warning, TEXT("Boost"));
			RobotCharacter->GetCharacterMovement()->Velocity.Z = 0;
			RobotCharacter->JumpMaxCount = 2;
			RobotCharacter->Jump();

			FTimerHandle MemberTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump, 1.0f,
			                                       false);
		}

		if (Actor->ActorHasTag(FName("Enemy")))
		{
			Cast<AEnemyCharacter>(Actor)->Stun(3.0f);
			UE_LOG(LogTemp, Warning, TEXT("Stun"));
		}
		
		if (Actor->ActorHasTag(FName("Grenade")))
		{
			Cast<ALightGrenade>(Actor)->ServerRPCExplosion();
			UE_LOG(LogTemp, Warning, TEXT("Explode"));
		}
	}

	RobotCharacter->OnPulse();
}

void URobotBaseState::ActivateAbilities()
{
	RobotCharacter->AbilityOne = true;
	RobotCharacter->AbilityTwo = true;
}
