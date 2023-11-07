// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DepthMovementEnabler.generated.h"

class APROJCharacter;

UCLASS()
class PROJ_API ADepthMovementEnabler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADepthMovementEnabler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	class UBoxComponent* TriggerBox; 

	UFUNCTION() 
	void OnComponentBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION() 
	void OnComponentStopOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								   UPrimitiveComponent* OtherComponent, int OtherBodyIndex); 

	void ToggleDepthMovement(APROJCharacter* Player, const bool bNewToggle) const; 

};
