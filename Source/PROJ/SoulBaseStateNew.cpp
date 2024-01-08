// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulBaseStateNew.h"

#include "BasicAttackComponent.h"
#include "CharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "LightGrenade.h"
#include "PROJCharacter.h"
#include "ShadowSoulCharacter.h"
#include "SoulCharacter.h"
#include "SoulDashingState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

USoulBaseStateNew::USoulBaseStateNew()
{
	SetIsReplicatedByDefault(true); 
}

void USoulBaseStateNew::BeginPlay()
{
	Super::BeginPlay();

	SoulCharacter = Cast<ASoulCharacter>(CharOwner);
	ShadowSoul = Cast<AShadowSoulCharacter>(CharOwner); 

	LightGrenade = Cast<ALightGrenade>(UGameplayStatics::GetActorOfClass(this, ALightGrenade::StaticClass()));

	AttackComponent = CharOwner->FindComponentByClass<UBasicAttackComponent>(); 
}

void USoulBaseStateNew::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	// Testing showed that there are 5 bindings set up before this is called (8 for some margin)
	if(InputComp->GetActionEventBindings().Num() < 8) 
	{
		InputComp->BindAction(DashInputAction, ETriggerEvent::Started, this, &USoulBaseStateNew::Dash);
		
		InputComp->BindAction(ThrowGrenadeInputAction,ETriggerEvent::Completed,this,&USoulBaseStateNew::ThrowGrenade);
		InputComp->BindAction(ThrowGrenadeInputAction,ETriggerEvent::Ongoing,this,&USoulBaseStateNew::GetTimeHeld);
		InputComp->BindAction(ThrowGrenadeInputAction, ETriggerEvent::Started, this, &USoulBaseStateNew::BeginGrenadeThrow);
		
		InputComp->BindAction(AbilityInputAction,ETriggerEvent::Started,this,&USoulBaseStateNew::ActivateAbilities);
	}
}

void USoulBaseStateNew::Exit()
{
	Super::Exit();

	bDashCoolDownActive = true;
	ServerRPC_EnableDashCooldown(); 

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USoulBaseStateNew::ServerRPC_DisableDashCooldown, DashCooldown); 
}

void USoulBaseStateNew::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void USoulBaseStateNew::Dash()
{
	// Only run locally 
	if(bDashCoolDownActive || !CharOwner->IsLocallyControlled()) 
		return;

	// controlled by player and ability is not unlocked 
	if(CharOwner->IsPlayerControlled() && !SoulCharacter->AbilityOne)
		return; 

	if(CharOwner->IsPlayerControlled())
	{
		if(bHasBeganThrow)
		{
			TimeHeld = 0; 
			ThrowGrenade();
		}
		SoulCharacter->SwitchState(SoulCharacter->DashingState);
	}
	else 
		ShadowSoul->SwitchState(ShadowSoul->DashState); 
}

void USoulBaseStateNew::ServerRPC_EnableDashCooldown_Implementation()
{
	bDashCoolDownActive = true; 
}

void USoulBaseStateNew::ServerRPC_DisableDashCooldown_Implementation()
{
	bDashCoolDownActive = false; 
}

void USoulBaseStateNew::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USoulBaseStateNew, TimeHeld);
	DOREPLIFETIME(USoulBaseStateNew, bDashCoolDownActive);
}

void USoulBaseStateNew::GetTimeHeld(const FInputActionInstance& Instance)
{
	if (!CharOwner->IsLocallyControlled() || !SoulCharacter->AbilityTwo)
	{
		EndGrenadeThrowWithoutThrowing(); 
		return;	
	}

	if(!bHasBeganThrow)
		return; 

	// UE_LOG(LogTemp, Warning, TEXT("TimeHeld() local - OnGoing IA: Time: %f"), Instance.GetElapsedTime())

	if(LightGrenade)
		LightGrenade->IsChargingGrenade(Instance.GetElapsedTime()); 

	// GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "IS Holding " + FString::SanitizeFloat(Instance.GetElapsedTime()));
	TimeHeld = Instance.GetElapsedTime();
}

void USoulBaseStateNew::ThrowGrenade()
{
	if (!CharOwner->IsLocallyControlled() || !SoulCharacter->AbilityTwo || !bHasBeganThrow)
	{
		return;	
	}
	
	bHasBeganThrow = false; 

	AttackComponent->ToggleAttackEnable(true); 

	CheckCanThrow();
}

void USoulBaseStateNew::BeginGrenadeThrow()
{
	
	
	
	if (!SoulCharacter->AbilityTwo)
		return;
	
	if(!LightGrenade){
		SoulCharacter->SpawnGrenade();
		LightGrenade = Cast<ALightGrenade>(UGameplayStatics::GetActorOfClass(this, ALightGrenade::StaticClass()));
	}

	if (!LightGrenade->bCanThrow)
		return;

	
	bHasBeganThrow = true; 
	AttackComponent->ToggleAttackEnable(false);
	ServerRPC_BeginGrenadeThrow(); 
	
}

void USoulBaseStateNew::MulticastRPC_BeginGrenadeThrow_Implementation()
{
	SoulCharacter->OnGrenadeThrowStart();
}

void USoulBaseStateNew::ServerRPC_BeginGrenadeThrow_Implementation()
{
	if(!CharOwner->HasAuthority())
		return;
	
	MulticastRPC_BeginGrenadeThrow(); 
}

void USoulBaseStateNew::ServerRPCThrowGrenade_Implementation(const float TimeHeldGrenade)
{
	if(!CharOwner->HasAuthority())
		return;
	

	MulticastRPCThrowGrenade(TimeHeldGrenade);
}

void USoulBaseStateNew::MulticastRPCThrowGrenade_Implementation(const float TimeHeldGrenade)
{
	if(LightGrenade)
		LightGrenade->Throw(TimeHeldGrenade);
	
	SoulCharacter->OnGrenadeThrowEnd(); 
}

void USoulBaseStateNew::ActivateAbilities()
{
	SoulCharacter->AbilityOne = true;
	SoulCharacter->AbilityTwo = true;
}

void USoulBaseStateNew::CheckCanThrow()
{
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult,SoulCharacter->GetActorLocation(),SoulCharacter->ThrowLoc->GetComponentLocation(),ECC_Visibility))
	{
		if(LightGrenade)
			LightGrenade->DisableIndicator();
		
		SoulCharacter->OnGrenadeThrowEnd(); 
	}
	else
	{
		ServerRPCThrowGrenade(TimeHeld);
	}
	
}

void USoulBaseStateNew::EndGrenadeThrowWithoutThrowing()
{
	if(LightGrenade)
		LightGrenade->DisableIndicator();
	
	ServerRPCThrowGrenade(-1); // -1 will call make it so grenade is not thrown but animation is exited
	AttackComponent->ToggleAttackEnable(true); 
	bHasBeganThrow = false; 
}
