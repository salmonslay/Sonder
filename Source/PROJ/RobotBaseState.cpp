// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotBaseState.h"

#include "CollisionDebugDrawingPublic.h"
#include "EnemyCharacter.h"
#include "EnhancedInputComponent.h"
#include "LightGrenade.h"
#include "RobotHookingState.h"
#include "PulseObjectComponent.h"
#include "RobotStateMachine.h"
#include "ShadowRobotCharacter.h"
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
		RobotCharacter = Cast<ARobotStateMachine>(CharOwner);

	DefaultWalkSpeed = CharOwner->GetCharacterMovement()->MaxWalkSpeed;
}

void URobotBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	// Testing showed that there are 5 bindings set up before this is called (7 for some margin)
	if (InputComp->GetActionEventBindings().Num() < 7)
	{
		InputComp->BindAction(HookShotInputAction, ETriggerEvent::Started, this, &URobotBaseState::ShootHook);

		InputComp->BindAction(PulseInputAction, ETriggerEvent::Started, this, &URobotBaseState::Pulse);

		InputComp->BindAction(AbilityInputAction, ETriggerEvent::Started, this, &URobotBaseState::ActivateAbilities); 
	}
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
	if (!CharOwner->HasAuthority())
		return;

	bHasDashBuff = true;

	MulticastRPC_DashBuffStart();
}

void URobotBaseState::MulticastRPC_DashBuffStart_Implementation()
{
	CharOwner->GetCharacterMovement()->MaxWalkSpeed = WalkSpeedWhenBuffed;

	if(CharOwner->IsPlayerControlled())
		RobotCharacter->OnDashBuffStart();
	else
		Cast<AShadowRobotCharacter>(CharOwner)->OnDashBuffStart(); 
}

void URobotBaseState::ResetDashBuff()
{
	ServerRPC_DashBuffEnd();
}

void URobotBaseState::ServerRPC_DashBuffEnd_Implementation()
{
	if (!CharOwner->HasAuthority())
		return;

	bHasDashBuff = false;

	MulticastRPC_DashBuffEnd();
}

void URobotBaseState::MulticastRPC_DashBuffEnd_Implementation()
{
	CharOwner->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	if(CharOwner->IsPlayerControlled())
		RobotCharacter->OnDashBuffEnd();
	else
		Cast<AShadowRobotCharacter>(CharOwner)->OnDashBuffEnd(); 
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

	Cast<ACharacterStateMachine>(CharOwner)->SwitchState(RobotCharacter->HookState);
}

void URobotBaseState::Pulse()
{
	if(bPulseCoolDownActive || !CharOwner->IsLocallyControlled()) 
		return;

	// controlled by player and ability is not unlocked 
	if(CharOwner->IsPlayerControlled() && !RobotCharacter->AbilityOne)
		return; 
	
	bPulseCoolDownActive = true;

	FTimerHandle PulseTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(PulseTimerHandle, this, &URobotBaseState::DisablePulseCooldown,
	                                       PulseCooldown);

	ServerRPCPulse();
}

void URobotBaseState::ServerRPCPulse_Implementation()
{
	// Should only run on server 
	if (!GetOwner()->HasAuthority())
		return;

	MulticastRPCPulse();
}

void URobotBaseState::MulticastRPCPulse_Implementation()
{
	// Code here is run on each player (client and server)
	TArray<AActor*> OverlappingActors;
	CharOwner->GetOverlappingActors(OverlappingActors, AActor::StaticClass());

	for (const auto Actor : OverlappingActors)
	{
		if(const auto Soul = Cast<ASoulCharacter>(Actor))
		{
			// See if there is line of sight to Soul, if there isn't then do nothing with Soul 
			FHitResult HitResult; 
			if(GetWorld()->LineTraceSingleByChannel(HitResult, CharOwner->GetActorLocation(), Soul->GetActorLocation(), ECC_Pawn))
				continue;
			
			if(CharOwner->GetActorLocation().Y - 100 < Actor->GetActorLocation().Y && Actor->GetActorLocation().Y < CharOwner->GetActorLocation().Y + 100)
			{
				if (Actor->GetActorLocation().Z > CharOwner->GetActorLocation().Z + 5)
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

				else if (Actor->GetActorLocation().Z + 20 < CharOwner->GetActorLocation().Z && CharOwner->GetCharacterMovement()->IsMovingOnGround() == false)
				{
					PlayerActor = RobotCharacter; 

					CharOwner->GetCharacterMovement()->Velocity.Z = 0;
					CharOwner->JumpMaxCount = 2;
					CharOwner->Jump();

					FTimerHandle MemberTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump, 1.0f);
				}
			}
		}

		else if (const auto Enemy = Cast<AEnemyCharacter>(Actor))
		{
			Enemy->Stun(3.0f);
			Enemy->TakeDamage(Damage, FDamageEvent(), Controller, CharOwner);
			UE_LOG(LogTemp, Warning, TEXT("Stun"));
		}
		
		else if (const auto Grenade = Cast<ALightGrenade>(Actor))
		{
			Grenade->PulseExplosion();
			Grenade->ServerRPCExplosion();
			UE_LOG(LogTemp, Warning, TEXT("Explode"));
		}

		else
			Actor->TakeDamage(Damage, FDamageEvent(), Controller, CharOwner);
	}
	
	
	if (CharOwner->IsPlayerControlled())
	{
		RobotCharacter->OnPulse();
	}
	else
	{
		if (const auto ShadowRobot = Cast<AShadowRobotCharacter>(CharOwner))
		{
			ShadowRobot->OnPulse();
		}
	}
}

void URobotBaseState::ActivateAbilities()
{
	RobotCharacter->AbilityOne = true;
	RobotCharacter->AbilityTwo = true;
}
