// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAttackComponent.h"

#include "PROJCharacter.h"
#include "ShadowCharacter.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBasicAttackComponent::UBasicAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Note Tick is turned off 
	
	SetRelativeLocation(FVector(50, 0, 0)); 
	SetRelativeScale3D(FVector(1.5f, 1.5f, 2.f));
	
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_Pawn);
	SetCollisionResponseToAllChannels(ECR_Overlap); 
}

void UBasicAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());

	bCanAttack = true;
	LastTimeAttack = 0; 
}

void UBasicAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	LastTimeAttack = 0;
	bCanAttack = true; 

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

bool UBasicAttackComponent::Attack()
{
	if(!GetWorld() || !bAttackEnabled)
		return false; 
	
	if(GetWorld()->TimeSeconds - AttackCooldown >= LastTimeAttack)
		bCanAttack = true;
	
	// Ensure player cant spam attack and is locally controlled 
	if(!bCanAttack || !Owner->IsLocallyControlled())
		return false;

	LastTimeAttack = GetWorld()->TimeSeconds;
	
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBasicAttackComponent::EnableCanAttack, AttackCooldown);

	// Run server function which will update each client and itself 
	ServerRPCAttack();

	return true; 
}

void UBasicAttackComponent::ServerRPCAttack_Implementation()
{
	// Should only run on server 
	if(!GetOwner()->HasAuthority())
		return; 
	
	MulticastRPCAttack(); 
}

void UBasicAttackComponent::MulticastRPCAttack_Implementation()
{
	// Sometimes attack is fired before player is set when loading new level, ensure player is set 
	if(!Owner)
	{
		Owner = Cast<ACharacter>(GetOwner()); 
		return;
	}

	bool bCalledHitEvent = false;

	const auto PlayerOwner = Cast<APROJCharacter>(Owner);
	const auto EnemyOwner = Cast<AShadowCharacter>(Owner); 

	if(ShouldCallHitEvent(nullptr))
	{
		if(PlayerOwner)
			PlayerOwner->OnBasicAttackHit();
		else if(EnemyOwner)
			EnemyOwner->OnBasicAttackHit();
		
		bCalledHitEvent = true; 
	}
	
	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, AActor::StaticClass()); 

	// TODO: When the attack animation in in place, we may want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
	{
		const bool bDamagingPlayer = Actor->IsA(APROJCharacter::StaticClass()); 
		
		// Player: damage all but other players, AI controlled: damage only players 
		if((Owner->IsPlayerControlled() && !bDamagingPlayer) || (!Owner->IsPlayerControlled() && bDamagingPlayer)) 
		{
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
			
			if(!bCalledHitEvent && ShouldCallHitEvent(Actor)) 
			{
				if(Owner->IsPlayerControlled() && PlayerOwner)
					PlayerOwner->OnBasicAttackHit();
				
				else if(!Owner->IsPlayerControlled() && EnemyOwner)
					EnemyOwner->OnBasicAttackHit();
				
				bCalledHitEvent = true; 
			}
		}
	}

	bCanAttack = false; 

	if(Owner->IsPlayerControlled() && PlayerOwner)
		PlayerOwner->OnBasicAttack();
	else if(!Owner->IsPlayerControlled() && EnemyOwner)
		EnemyOwner->OnBasicAttack();
}

bool UBasicAttackComponent::ShouldCallHitEvent(AActor* OverlappingActor) const
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

void UBasicAttackComponent::EnableCanAttack()
{
	bCanAttack = true; 
}
