// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructableBox.generated.h"

UCLASS()
class PROJ_API ADestructableBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructableBox();

	/** Property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

protected:

	virtual void Tick(float DeltaSeconds) override;

private:
	
	/** The geometry collision that can break apart */
	UPROPERTY(EditDefaultsOnly)
	class UGeometryCollectionComponent* DestructibleBox;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollBox; 

	UPROPERTY(VisibleAnywhere, Category=Health, Replicated)
	class UBaseHealthComponent* HealthComponent = nullptr;

	bool bDestroyed = false;

	UPROPERTY(EditAnywhere)
	float ShrinkSpeed = 10.f;

	/** Force to apply to the box when it is destroyed, determines how much the pieces scatter */
	UPROPERTY(EditAnywhere)
	float DestroyedForce = 20000.f; 

	/** Local crumble */
	void Crumble();

	UFUNCTION(Server, Reliable)
	void ServerRPC_Crumble();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Crumble(); 

	void Shrink(const float DeltaTime); 
	
};
