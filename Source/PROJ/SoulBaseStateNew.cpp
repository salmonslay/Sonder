// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulBaseStateNew.h"

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

void USoulBaseStateNew::Enter()
{
	Super::Enter();

	if(!SoulCharacter)
		SoulCharacter = Cast<ASoulCharacter>(CharOwner);

	if(!LightGrenade)
		LightGrenade = Cast<ALightGrenade>(UGameplayStatics::GetActorOfClass(this, ALightGrenade::StaticClass())); 
}

void USoulBaseStateNew::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	if(!bHasSetUpInput)
	{
		InputComp->BindAction(DashInputAction, ETriggerEvent::Started, this, &USoulBaseStateNew::Dash);
		InputComp->BindAction(ThrowGrenadeInputAction,ETriggerEvent::Completed,this,&USoulBaseStateNew::ThrowGrenade);
		InputComp->BindAction(ThrowGrenadeInputAction,ETriggerEvent::Ongoing,this,&USoulBaseStateNew::GetTimeHeld);
		InputComp->BindAction(AbilityInputAction,ETriggerEvent::Started,this,&USoulBaseStateNew::ActivateAbilities);
		bHasSetUpInput = true; 
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
		Cast<ACharacterStateMachine>(CharOwner)->SwitchState(SoulCharacter->DashingState);
	else if(const auto ShadowSoul = Cast<AShadowSoulCharacter>(CharOwner))
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
		return;	
	}

	// UE_LOG(LogTemp, Warning, TEXT("TimeHeld() local - OnGoing IA: Time: %f"), Instance.GetElapsedTime())

	if(LightGrenade)
		LightGrenade->IsChargingGrenade(Instance.GetElapsedTime()); 

	// GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "IS Holding " + FString::SanitizeFloat(Instance.GetElapsedTime()));
	TimeHeld = Instance.GetElapsedTime();
}

void USoulBaseStateNew::ThrowGrenade()
{
	if (!CharOwner->IsLocallyControlled() || !SoulCharacter->AbilityTwo)
	{
		return;	
	}

	ServerRPCThrowGrenade(TimeHeld);
	
}

void USoulBaseStateNew::ServerRPCThrowGrenade_Implementation(const float TimeHeldGrenade)
{
	if(!CharOwner->HasAuthority())
		return;
	
	//LightGrenade = GetWorld()->SpawnActor<AActor>(LightGrenadeRef,SoulCharacter->FireLoc->GetComponentLocation(),SoulCharacter->FireLoc->GetComponentRotation());

	MulticastRPCThrowGrenade(TimeHeldGrenade);
}

void USoulBaseStateNew::MulticastRPCThrowGrenade_Implementation(const float TimeHeldGrenade)
{
	if(LightGrenade)
		LightGrenade->Throw(TimeHeldGrenade);
}

void USoulBaseStateNew::ActivateAbilities()
{
	SoulCharacter->AbilityOne = true;
	SoulCharacter->AbilityTwo = true;
}
