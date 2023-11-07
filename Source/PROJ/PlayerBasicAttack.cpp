// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBasicAttack.h"

#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "Engine/DamageEvents.h"

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

// TODO: This whole function is only run locally rn, make sure what needs to be networked, is networked 
void UPlayerBasicAttack::Attack()
{
	if(!bCanAttack)
		return; 
	
	UE_LOG(LogTemp, Warning, TEXT("Attack!"))

	Player->OnBasicAttack(); 

	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, AActor::StaticClass()); // TODO: Replace the class filter with eventual better class (if it exists)

	// calls take damage on every overlapping actor except itself
	// TODO: When the attack animation in in place, we prob want to delay this so it times with when the animation hits 
	for(const auto Actor : OverlappingActors)
	{
		if(Actor != GetOwner())
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner()); 
	}

	bCanAttack = false; 
	
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPlayerBasicAttack::EnableCanAttack, AttackCooldown); 
}

void UPlayerBasicAttack::EnableCanAttack()
{
	bCanAttack = true; 
}
