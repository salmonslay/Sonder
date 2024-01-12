// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookExplosionActor.generated.h"

/**
 * This is the actor spawned when Robot collides with Soul after a hook shot 
 */
UCLASS()
class SONDER_API AHookExplosionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHookExplosionActor();

	/** Takes in spawn parameters, basically the constructor. Requires slightly different spawning */
	void Initialize(const float HookTravelDistance, APawn* RobotCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/**
	 * NOTE: This overlap component is not used to see overlapping actors. It is only there to serve as a visual
	 * when setting the radius. Additional modifications are not considered 
	 */
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* OverlapSphere;

	/** Damage to deal to overlapping actors per meter traveled with the hook shot. 100 Unreal Units = 1 meter */
	UPROPERTY(EditAnywhere)
	float ExplosionDamageAmountPerMeterTraveled = 3.f;

	/** Minimum damage that the explosion will do */
	UPROPERTY(EditAnywhere)
	float MinimumDamageAmount = 1.f; 

	/** Hook shot travel distance upon colliding with Soul. Set in Initialize() */
	float HookShotTravelDistance = 1;

	// Set in Initialize() 
	UPROPERTY()
	APawn* RobotChar;

	/** Explosion effect to display */
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* ExplosionNS;

	/** Object types to check for collision and damage. Prob Enemy unless we will have destructible objects */
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapCollisionObjects; 

	void DamageOverlap();

	float GetDamageAmount() const; 
	
};
