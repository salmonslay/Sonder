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
	if(!Owner)
	{
		Owner = Cast<ACharacter>(GetOwner()); 
		return false;
	}
	
	if(!GetWorld() || !bAttackEnabled || !bCanAttack || !Owner->IsLocallyControlled())
		return false; 
	
	if(GetWorld()->TimeSeconds - AttackCooldown >= LastTimeAttack)
		bCanAttack = true;

	LastTimeAttack = GetWorld()->TimeSeconds;
	
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBasicAttackComponent::EnableCanAttack, AttackCooldown);

	ServerRPC_StartAttack();
	
	bCanAttack = false; 

	return true; 
}

void UBasicAttackComponent::DoAttackDamage()
{
	// Ensure player is set 
	if(!Owner)
	{
		Owner = Cast<ACharacter>(GetOwner()); 
		return;
	}

	bool bCalledHitEvent = false;

	const auto PlayerOwner = Cast<APROJCharacter>(Owner);
	const auto EnemyOwner = Cast<AShadowCharacter>(Owner); 

	if(ShouldCallHitEvent())
	{
		if(PlayerOwner)
			PlayerOwner->OnBasicAttackHit();
		else if(EnemyOwner)
			EnemyOwner->OnBasicAttackHit();
		
		bCalledHitEvent = true; 
	}
	
	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, AActor::StaticClass());

	const bool bPlayerControlled = Owner->IsPlayerControlled(); 

	for(const auto Actor : OverlappingActors)
	{
		const bool bDamagingPlayer = Actor->IsA(APROJCharacter::StaticClass()); 
		
		// Player: damage all but other players, AI controlled: damage only players 
		if((bPlayerControlled && !bDamagingPlayer) || (!bPlayerControlled && bDamagingPlayer)) 
		{
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
			
			if(!bCalledHitEvent && ShouldCallHitEvent(Actor)) 
			{
				if(bPlayerControlled && PlayerOwner)
					PlayerOwner->OnBasicAttackHit();
				
				else if(!bPlayerControlled && EnemyOwner)
					EnemyOwner->OnBasicAttackHit();
				
				bCalledHitEvent = true; 
			}
		}
	}
}

bool UBasicAttackComponent::ShouldCallHitEvent(AActor* OverlappingActor) const
{
	// Pawns are considered hits even though they dont physically collide 
	if(OverlappingActor)
		return OverlappingActor->IsA(APawn::StaticClass()); 
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	// Perform line trace forwards to see if something blocks/the punch hit something physical
	FVector EndLoc = Owner->GetActorLocation() + Owner->GetActorForwardVector() * HitEventLengthCheck; 
	return GetWorld()->LineTraceSingleByChannel(HitResult, Owner->GetActorLocation(), EndLoc, ECC_Pawn, Params); 
}

void UBasicAttackComponent::ServerRPC_StartAttack_Implementation()
{
	if(!Owner->HasAuthority())
		return;

	MulticastRPC_StartAttack(); 
}

void UBasicAttackComponent::MulticastRPC_StartAttack_Implementation()
{
	if(Owner->IsPlayerControlled())
	{
		if(const auto PlayerOwner = Cast<APROJCharacter>(Owner))
			PlayerOwner->OnBasicAttack();
	}
	else if(!Owner->IsPlayerControlled())
	{
		if(const auto EnemyOwner = Cast<AShadowCharacter>(Owner))
			EnemyOwner->OnBasicAttack();
	}

	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBasicAttackComponent::DoAttackDamage, AttackAnimationDelay); 
}

void UBasicAttackComponent::EnableCanAttack()
{
	bCanAttack = true; 
}
