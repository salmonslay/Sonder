// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBasicAttack.h"

#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "ShadowCharacter.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerBasicAttack::UPlayerBasicAttack()
{
	PrimaryComponentTick.bCanEverTick = false; // Note tick is turned off 

}

void UPlayerBasicAttack::SetUpInput(UEnhancedInputComponent* InputComp)
{
	if(Cast<ACharacter>(GetOwner())->IsPlayerControlled())
		InputComp->BindAction(AttackInputAction, ETriggerEvent::Started, this, &UPlayerBasicAttack::Attack);
}

void UPlayerBasicAttack::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());
}

void UPlayerBasicAttack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void UPlayerBasicAttack::Attack()
{
	// Ensure player cant spam attack and is locally controlled 
	if(!bCanAttack || !Owner->IsLocallyControlled())
		return; 
	
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPlayerBasicAttack::EnableCanAttack, AttackCooldown);

	// Run server function which will update each client and itself 
	ServerRPCAttack(); 
}

void UPlayerBasicAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerBasicAttack,  bCanAttack);
}

void UPlayerBasicAttack::ServerRPCAttack_Implementation()
{
	// Should only run on server 
	if(!GetOwner()->HasAuthority())
		return; 
	
	MulticastRPCAttack(); 
}

void UPlayerBasicAttack::MulticastRPCAttack_Implementation()
{
	// Sometimes attack is fired before player is set when loading new level, ensure player is set 
	if(!Owner)
		return;

	bool bCalledHitEvent = false; 
	
	// Code here is run on each player (client and server)
	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, AActor::StaticClass()); // TODO: Replace the class filter with eventual better class (if it exists)

	// calls take damage on every overlapping actor except itself
	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
	{
		// Player, damage all but other players 
		if(Owner->IsPlayerControlled() && !Actor->ActorHasTag(FName("Player")))
		{
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
			
			if(!bCalledHitEvent)
			{
				Cast<APROJCharacter>(Owner)->OnBasicAttackHit();
				bCalledHitEvent = true; 
			}
		}

		// AI controlled, damage only players 
		else if(!Owner->IsPlayerControlled() && Actor->IsA(APROJCharacter::StaticClass()))
		{
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());

			if(!bCalledHitEvent)
			{
				Cast<AShadowCharacter>(Owner)->OnBasicAttackHit();
				bCalledHitEvent = true; 
			}
		}

	}

	bCanAttack = false; 

	if(Owner->IsPlayerControlled())
		Cast<APROJCharacter>(Owner)->OnBasicAttack();
	else
		Cast<AShadowCharacter>(Owner)->OnBasicAttack(); 
}

void UPlayerBasicAttack::EnableCanAttack()
{
	bCanAttack = true; 
}
