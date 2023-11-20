// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulBaseStateNew.h"

#include "CharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "LightGrenade.h"
#include "PROJCharacter.h"
#include "SoulCharacter.h"
#include "SoulDashingState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void USoulBaseStateNew::Enter()
{
	Super::Enter();

	if(!SoulCharacter)
		SoulCharacter = Cast<ASoulCharacter>(PlayerOwner);

	// UE_LOG(LogTemp, Warning, TEXT("Entered soul base state, lcl ctrl: %i"), PlayerOwner->IsLocallyControlled()) 
}

void USoulBaseStateNew::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	
}

void USoulBaseStateNew::UpdateInputCompOnEnter(UEnhancedInputComponent* InputComp)
{
	Super::UpdateInputCompOnEnter(InputComp);

	if(!bHasSetUpInput)
	{
		InputComp->BindAction(DashInputAction, ETriggerEvent::Started, this, &USoulBaseStateNew::Dash);
		InputComp->BindAction(ThrowGrenadeInputAction,ETriggerEvent::Completed,this,&USoulBaseStateNew::ThrowGrenade);
		InputComp->BindAction(ThrowGrenadeInputAction,ETriggerEvent::Ongoing,this,&USoulBaseStateNew::GetTimeHeld);
		bHasSetUpInput = true; 
	}
}

void USoulBaseStateNew::Exit()
{
	Super::Exit();

	bDashCoolDownActive = true;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USoulBaseStateNew::DisableDashCooldown, 1); 

	// Removing the action binding would require changing the action mapping 
	// PlayerInputComponent->RemoveActionBinding(DashInputAction, ETriggerEvent::Started); 
}

void USoulBaseStateNew::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void USoulBaseStateNew::Dash()
{
	// Only run locally 
	if(bDashCoolDownActive || !PlayerOwner->IsLocallyControlled())
		return;

	PlayerOwner->SwitchState(SoulCharacter->DashingState); 
}

void USoulBaseStateNew::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USoulBaseStateNew,  TimeHeld);
}

void USoulBaseStateNew::GetTimeHeld(const FInputActionInstance& Instance)
{
	if (!PlayerOwner->IsLocallyControlled())
	{
		return;	
	}
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "IS Holding " + FString::SanitizeFloat(Instance.GetElapsedTime()));
	//TimeHeld = Instance.GetElapsedTime();
	
}

void USoulBaseStateNew::ThrowGrenade()
{
	if (!PlayerOwner->IsLocallyControlled())
	{
		return;	
	}
	
	ServerRPCThrowGrenade();
	
}



void USoulBaseStateNew::ServerRPCThrowGrenade_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	
	//LightGrenade = GetWorld()->SpawnActor<AActor>(LightGrenadeRef,SoulCharacter->FireLoc->GetComponentLocation(),SoulCharacter->FireLoc->GetComponentRotation());

		
	
	
	MulticastRPCThrowGrenade();
}


void USoulBaseStateNew::MulticastRPCThrowGrenade_Implementation()
{
	TArray<AActor*> FoundCharacter;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightGrenade::StaticClass(), FoundCharacter);

	if (FoundCharacter[0] != nullptr)
	{
		ALightGrenade* Grenade = Cast<ALightGrenade>(FoundCharacter[0]);
		UE_LOG(LogTemp, Warning, TEXT("Time held %f"), TimeHeld);
		Grenade->Throw();
		
	}
	
}

