// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "PlayerBasicAttack.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_API UPlayerBasicAttack : public UBoxComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UPlayerBasicAttack();

	/** Called to connect input to attack function */
	void SetUpInput(UEnhancedInputComponent* InputComp);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(EditAnywhere)
	class UInputAction* AttackInputAction; 

	/** Damage inflicted on enemies */
	UPROPERTY(EditAnywhere)
	float Damage = 5.f;

	/** How long time needs to pass between attacks */
	UPROPERTY(EditAnywhere)
	float AttackCooldown = 0.5f; 

	UPROPERTY(Replicated)
	bool bCanAttack = true;

	void EnableCanAttack();

	UPROPERTY()
	class APROJCharacter* Player; 

	/** Locally run function called when player presses attack button */
	void Attack();
	
	/**
	 * Function executed on server ONLY when client attacks, called from client. This function will probably only call
	 * the multicast function unless we will have some functionality that should only be run on the server 
	 */ 
	UFUNCTION(Server, Reliable)
	void ServerRPCAttack(); 

	/** Attack function run on each game instance, client and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
};
