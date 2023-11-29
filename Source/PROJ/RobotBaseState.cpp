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
#include "Engine/DamageEvents.h"
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
	DOREPLIFETIME(URobotBaseState, bHookShotOnCooldown)
	DOREPLIFETIME(URobotBaseState, bPulseCoolDownActive)
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

	for (const auto Actor : OverlappingActors)
	{
		if(const auto Soul = Cast<ASoulCharacter>(Actor))
		{
			// See if there is line of sight to Soul, if there isn't then do nothing with Soul 
			FHitResult HitResult; 
			if(GetWorld()->LineTraceSingleByChannel(HitResult, RobotCharacter->GetActorLocation(), Soul->GetActorLocation(), ECC_Pawn))
				continue;
			
			if(RobotCharacter->GetActorLocation().Y - 100 < Actor->GetActorLocation().Y && Actor->GetActorLocation().Y < RobotCharacter->GetActorLocation().Y + 100)
			{
				if (Actor->GetActorLocation().Z > RobotCharacter->GetActorLocation().Z + 5)
				{
					PlayerActor = Soul;
					UE_LOG(LogTemp, Warning, TEXT("Boost"));

					if(Soul->GetCharacterMovement()->IsMovingOnGround() == false)
					{
						Soul->GetCharacterMovement()->Velocity.Z = 0;
						Soul->JumpMaxCount = 2;
						Soul->Jump();
					}

					FTimerHandle MemberTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump, 1.0f);
				}

				else if (Actor->GetActorLocation().Z + 20 < RobotCharacter->GetActorLocation().Z && RobotCharacter->GetCharacterMovement()->IsMovingOnGround() == false)
				{
					PlayerActor = RobotCharacter; 

					RobotCharacter->GetCharacterMovement()->Velocity.Z = 0;
					RobotCharacter->JumpMaxCount = 2;
					RobotCharacter->Jump();

					FTimerHandle MemberTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump, 1.0f);
				}
			}
		}

		else if (const auto Enemy = Cast<AEnemyCharacter>(Actor))
		{
			Enemy->Stun(3.0f);
			Enemy->TakeDamage(Damage, FDamageEvent(), Controller, RobotCharacter);
			UE_LOG(LogTemp, Warning, TEXT("Stun"));
		}
		
		else if (const auto Grenade = Cast<ALightGrenade>(Actor))
		{
			Grenade->PulseExplosion();
			Grenade->ServerRPCExplosion();
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
