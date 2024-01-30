// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotBaseState.h"

#include "CollisionDebugDrawingPublic.h"
#include "DestructableBox.h"
#include "EnemyCharacter.h"
#include "EnhancedInputComponent.h"
#include "LightGrenade.h"
#include "RobotHookingState.h"
#include "RobotStateMachine.h"
#include "ShadowRobotCharacter.h"
#include "SonderSaveGame.h"
#include "SoulCharacter.h"
#include "Chaos/CollisionResolutionUtil.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

URobotBaseState::URobotBaseState()
{
	SetIsReplicatedByDefault(true);
}

void URobotBaseState::BeginPlay()
{
	Super::BeginPlay();

	RobotCharacter = Cast<ARobotStateMachine>(CharOwner);
	ShadowRobot = Cast<AShadowRobotCharacter>(CharOwner);

	DefaultWalkSpeed = CharOwner->GetCharacterMovement()->MaxWalkSpeed;

	MovementComponent = CharOwner->GetCharacterMovement();

	PulseCollision = CharOwner->FindComponentByClass<USphereComponent>();

	SetPulseCooldown(); 
}

void URobotBaseState::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	// Testing showed that there are 5 bindings set up before this is called (7 for some margin)
	if (InputComp->GetActionEventBindings().Num() < 7)
	{
		InputComp->BindAction(HookShotInputAction, ETriggerEvent::Started, this, &URobotBaseState::ShootHook);

		InputComp->BindAction(PulseInputAction, ETriggerEvent::Started, this, &URobotBaseState::Pulse);
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
	MovementComponent->MaxWalkSpeed = WalkSpeedWhenBuffed;

	if (CharOwner->IsPlayerControlled())
	{
		RobotCharacter->OnDashBuffStart();
		USonderSaveGame::AddRobotBoostsWithDash();
	}
	else
	{
		ShadowRobot->OnDashBuffStart();
	}
}

void URobotBaseState::ServerRPC_DisablePulseCooldown_Implementation()
{
	bPulseCoolDownActive = false; 
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
	MovementComponent->MaxWalkSpeed = DefaultWalkSpeed;

	if (CharOwner->IsPlayerControlled())
		RobotCharacter->OnDashBuffEnd();
	else
		ShadowRobot->OnDashBuffEnd();
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

	bHookShotOnCooldown = true;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &URobotBaseState::DisableHookShotCooldown,
	                                       HookShotCooldownDelay);

	RobotCharacter->SwitchState(RobotCharacter->HookState);
}

void URobotBaseState::Pulse()
{
	if (bPulseCoolDownActive || !CharOwner->IsLocallyControlled())
		return;

	// controlled by player and ability is not unlocked 
	if (CharOwner->IsPlayerControlled() && !RobotCharacter->AbilityOne)
		return;

	bPulseCoolDownActive = true;

	FTimerHandle PulseTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(PulseTimerHandle, this, &URobotBaseState::ServerRPC_DisablePulseCooldown,
	                                       PulseCooldown);

	ServerRPCPulse();
}

void URobotBaseState::ServerRPCPulse_Implementation()
{
	if (!GetOwner()->HasAuthority())
		return;

	bPulseCoolDownActive = true;

	MulticastRPCPulse();
}

void URobotBaseState::MulticastRPCPulse_Implementation()
{
	const bool bIsPlayerControlled = CharOwner->IsPlayerControlled();
	
	TArray<AActor*> OverlappingActors;
	if(bIsPlayerControlled)
		PulseCollision->GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	else // AI/Enemy, only find players 
		PulseCollision->GetOverlappingActors(OverlappingActors, APROJCharacter::StaticClass());

	FHitResult HitResult;
	const FVector OwnerLoc = CharOwner->GetActorLocation(); 
	
	for (const auto Actor : OverlappingActors)
	{
		const FVector OverlapActorLoc = Actor->GetActorLocation(); 
		
		// See if there is line of sight to actor
		if(!Actor->IsA(ADestructableBox::StaticClass()))
		{
			FCollisionQueryParams Params;
			Params.AddIgnoredActors(TArray<AActor*> ({ CharOwner, Actor }));
			if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerLoc, OverlapActorLoc, ECC_Pawn, Params))
				continue;
		}
		
		if (!bIsPlayerControlled)
		{
			Actor->TakeDamage(Damage, FDamageEvent(), Controller, CharOwner);
			continue;
		}

		if (const auto Soul = Cast<ASoulCharacter>(Actor))
		{
			if (OwnerLoc.Y - 100 < OverlapActorLoc.Y && OverlapActorLoc.Y <
				OwnerLoc.Y + 100)
			{
				if (OverlapActorLoc.Z > OwnerLoc.Z + 5)
				{
					PlayerActor = Soul;
					UE_LOG(LogTemp, Warning, TEXT("Boost"));

					if (Soul->GetCharacterMovement()->IsMovingOnGround() == false)
					{
						Soul->GetCharacterMovement()->Velocity.Z = 0;
						Soul->JumpMaxCount = 2;
						Soul->Jump();
					}

					FTimerHandle MemberTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump,
					                                       1.0f);
				}

				else if (OverlapActorLoc.Z + 20 < OwnerLoc.Z && MovementComponent->
					IsMovingOnGround() == false)
				{
					PlayerActor = RobotCharacter;

					MovementComponent->Velocity.Z = 0;
					CharOwner->JumpMaxCount = 2;
					CharOwner->Jump();

					FTimerHandle MemberTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &URobotBaseState::DisableSecondJump,
					                                       1.0f);
				}
			}
		}

		else if (const auto Enemy = Cast<AEnemyCharacter>(Actor))
		{
			Enemy->Stun(2.0f);
			Enemy->TakeDamage(Damage, FDamageEvent(), Controller, CharOwner);
			UE_LOG(LogTemp, Warning, TEXT("Stun"));
		}

		else if (const auto Grenade = Cast<ALightGrenade>(Actor))
		{
			Grenade->PulseExplosion();
			USonderSaveGame::AddGrenadesExplodedWithPulse();
			Grenade->ServerRPCExplosion();
			UE_LOG(LogTemp, Warning, TEXT("Explode"));
		}

		else
		{
			Actor->TakeDamage(Damage, FDamageEvent(), Controller, CharOwner);
		}
	}

	if (CharOwner->IsPlayerControlled())
		RobotCharacter->OnPulse();
	else
		ShadowRobot->OnPulse();
}

void URobotBaseState::SetPulseCooldown()
{
	if(RobotCharacter && UGameplayStatics::GetCurrentLevelName(this).Equals(TEXT("ArenaSurvival")))
		PulseCooldown = PulseCooldownInArena;
}
