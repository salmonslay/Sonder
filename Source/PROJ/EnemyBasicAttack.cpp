// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBasicAttack.h"

#include "PROJCharacter.h"
#include "ShadowCharacter.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UEnemyBasicAttack::UEnemyBasicAttack()
{
	PrimaryComponentTick.bCanEverTick = false; // Note tick is turned off 

}

void UEnemyBasicAttack::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AShadowCharacter>(GetOwner());
}

void UEnemyBasicAttack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void UEnemyBasicAttack::Attack()
{
	// Ensure player cant spam attack and is locally controlled 
	if(!bCanAttack || !Owner->IsLocallyControlled())
		return; 
	
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEnemyBasicAttack::EnableCanAttack, AttackCooldown);

	// Run server function which will update each client and itself 
	ServerRPCAttack(); 
}

void UEnemyBasicAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEnemyBasicAttack,  bCanAttack);
}

void UEnemyBasicAttack::ServerRPCAttack_Implementation()
{
	// Should only run on server 
	if(!GetOwner()->HasAuthority())
		return; 
	
	MulticastRPCAttack(); 
}

void UEnemyBasicAttack::MulticastRPCAttack_Implementation()
{
	// Sometimes attack is fired before player is set when loading new level, ensure player is set 
	if(!Owner)
		return;
	
	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, APROJCharacter::StaticClass()); 

	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
		Actor->TakeDamage(Damage, FDamageEvent(), Owner->GetInstigatorController(), Owner); 

	bCanAttack = false; 

	Owner->OnBasicAttack();
}

void UEnemyBasicAttack::EnableCanAttack()
{
	bCanAttack = true; 
}
