// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBasicAttack.h"

#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerBasicAttack::UPlayerBasicAttack()
{
	PrimaryComponentTick.bCanEverTick = false; // Note tick is turned off 

}

void UPlayerBasicAttack::SetUpInput(UEnhancedInputComponent* InputComp)
{
	InputComp->BindAction(AttackInputAction, ETriggerEvent::Started, this, &UPlayerBasicAttack::Attack);
}

void UPlayerBasicAttack::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APROJCharacter>(GetOwner());
}

void UPlayerBasicAttack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void UPlayerBasicAttack::Attack()
{
	// Code here is only run locally 
	
	// Ensure player cant spam attack and is locally controlled 
	if(!bCanAttack || !Player->IsLocallyControlled())
		return; 
	
	// UE_LOG(LogTemp, Warning, TEXT("Local attack"))
	
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
	// Code here is only run on server, will probably not be changed unless we'll have server specific behaviour 
	
	// Should only run on server 
	if(!GetOwner()->HasAuthority())
		return; 

	// UE_LOG(LogTemp, Warning, TEXT("Server attack"))
	
	MulticastRPCAttack(); 
}

void UPlayerBasicAttack::MulticastRPCAttack_Implementation()
{
	
	// Code here is run on each player (client and server)
	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, AActor::StaticClass()); // TODO: Replace the class filter with eventual better class (if it exists)

	// calls take damage on every overlapping actor except itself
	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
	{
		// Friendly fire off 
		if(!Actor->ActorHasTag(FName("Player")))
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner()); 
	}

	bCanAttack = false; 
	
	// UE_LOG(LogTemp, Warning, TEXT("Multicast attack"))

	Player->OnBasicAttack(); 
}

void UPlayerBasicAttack::EnableCanAttack()
{
	bCanAttack = true; 
}
