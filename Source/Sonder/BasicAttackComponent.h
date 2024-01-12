// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BasicAttackComponent.generated.h"

/**
 * 
 */
UCLASS()
class SONDER_API UBasicAttackComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UBasicAttackComponent();

	/** Try to trigger attack, returns true if attack could be performed */
	bool Attack();

	bool CanAttack() const { return bCanAttack; }

	void ToggleAttackEnable(const bool bEnabled) { bAttackEnabled = bEnabled; }

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
	ACharacter* Owner;

	// Completely disabled, not cooldown (e.g. when throwing grenade)
	bool bAttackEnabled = true;

	/** How long to delay the attack after pressing attack button to sync with animation */
	UPROPERTY(EditAnywhere)
	float AttackAnimationDelay = 0.17f;

	UPROPERTY()
	class APROJCharacter* PlayerOwner;

	UPROPERTY()
	class AShadowCharacter* ShadowOwner; 

	/** Initiates the attack and syncs it with the punch animation */
	UFUNCTION(Server, Reliable)
	void ServerRPC_StartAttack(); 

	/** Initiates the attack and syncs it with the punch animation */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_StartAttack();

	/** Performs the actual attack and deals damage */ 	
	void DoAttackDamage();

	void EnableCanAttack() { bCanAttack = true; }

	/** If an actor is passed, then it is checked if it's a pawn. Otherwise a line trace is performed */
	bool ShouldCallHitEvent(AActor* OverlappingActor = nullptr) const;
	
};
