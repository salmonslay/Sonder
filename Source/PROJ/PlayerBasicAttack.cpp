// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBasicAttack.h"

#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "ShadowCharacter.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
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

	bCanAttack = true;
	LastTimeAttack = 0; 
}

void UPlayerBasicAttack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	LastTimeAttack = 0;
	bCanAttack = true; 

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void UPlayerBasicAttack::Attack()
{
	if(!GetWorld())
		return; 
	
	if(GetWorld()->TimeSeconds - AttackCooldown >= LastTimeAttack)
		bCanAttack = true;

	if(!LastLevelName.Equals(UGameplayStatics::GetCurrentLevelName(this)))
		bCanAttack = true; 
	
	// Ensure player cant spam attack and is locally controlled 
	if(!bCanAttack || !Owner->IsLocallyControlled())
		return;

	LastTimeAttack = GetWorld()->TimeSeconds;
	LastLevelName = UGameplayStatics::GetCurrentLevelName(this); 
	
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPlayerBasicAttack::EnableCanAttack, AttackCooldown);

	// Run server function which will update each client and itself 
	ServerRPCAttack(); 
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
	{
		Owner = Cast<ACharacter>(GetOwner());
		return;
	}

	bool bCalledHitEvent = false;

	if(ShouldCallHitEvent(nullptr))
	{
		if(Owner->IsPlayerControlled())
			Cast<APROJCharacter>(Owner)->OnBasicAttackHit();
		else
			Cast<AShadowCharacter>(Owner)->OnBasicAttackHit();
		
		bCalledHitEvent = true; 
	}
	
	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, AActor::StaticClass()); 

	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
	{
		const bool bDamagingPlayer = Actor->IsA(APROJCharacter::StaticClass()); 
		
		// Player: damage all but other players, AI controlled: damage only players 
		if((Owner->IsPlayerControlled() && !bDamagingPlayer) || (!Owner->IsPlayerControlled() && bDamagingPlayer)) 
		{
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
			
			if(!bCalledHitEvent && ShouldCallHitEvent(Actor)) 
			{
				if(Owner->IsPlayerControlled())
					Cast<APROJCharacter>(Owner)->OnBasicAttackHit();
				if(!Owner->IsPlayerControlled())
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

bool UPlayerBasicAttack::ShouldCallHitEvent(AActor* OverlappingActor) const
{
	// Pawns are considered hits even though they dont physically collide 
	if(OverlappingActor && OverlappingActor->IsA(APawn::StaticClass()))
		return true; 
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	// Perform line trace forwards to see if something blocks/the punch hit something physical
	FVector EndLoc = Owner->GetActorLocation() + Owner->GetActorForwardVector() * HitEventLengthCheck; 
	return GetWorld()->LineTraceSingleByChannel(HitResult, Owner->GetActorLocation(), EndLoc, ECC_Pawn, Params); 
}

void UPlayerBasicAttack::EnableCanAttack()
{
	bCanAttack = true; 
}
