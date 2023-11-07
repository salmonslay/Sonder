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

void UPlayerBasicAttack::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack!"))

	TArray<AActor*> OverlappingActors; 
	GetOverlappingActors(OverlappingActors, AActor::StaticClass()); // TODO: Replace the class filter with eventual better class 

	for(const auto Actor : OverlappingActors)
	{
		if(Actor != GetOwner())
			Actor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner()); 
	}
}
