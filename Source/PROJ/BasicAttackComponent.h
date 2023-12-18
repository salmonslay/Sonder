// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BasicAttackComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBasicAttackComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UBasicAttackComponent();

	/** Locally run function called when player presses attack button, returns true if attack could be performed */
	bool Attack();

	bool CanAttack() const { return bCanAttack; }

	void EnableAttack() { bCanAttack = true; }

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/** Damage inflicted on enemies */
	UPROPERTY(EditAnywhere)
	float Damage = 5.f;

	/** How long time needs to pass between attacks */
	UPROPERTY(EditAnywhere)
	float AttackCooldown = 0.5f; 

	bool bCanAttack = true;

	/** How far forwards to see if something is blocking and thus call the hit event */
	UPROPERTY(EditAnywhere)
	float HitEventLengthCheck = 100.f; 

	UPROPERTY()
	class ACharacter* Owner;

	// Hopefully temporary, possible fixes for attack 
	float LastTimeAttack;
	
	/** Function executed on server ONLY on attacks, called from client */ 
	UFUNCTION(Server, Reliable)
	void ServerRPCAttack(); 

	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack();

	void EnableCanAttack();

	bool ShouldCallHitEvent(AActor* OverlappingActor) const;
	
};
