// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SwapSpline.generated.h"

class ACameraSpline;
class ACharactersCamera;

UCLASS()
class PROJ_API ASwapSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwapSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere)
	//ACameraSpline* CurrentCameraSpline;

	UPROPERTY(EditAnywhere)
	ACameraSpline* NextCameraSpline;

	UPROPERTY(EditAnywhere)
	ACharactersCamera* CharactersCamera;

	UFUNCTION(BlueprintCallable)
	void SplineSwap();
	

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UBoxComponent* TriggerZone;

private:

	UPROPERTY(EditAnywhere)
	double TransitionSpeed;
	
	


};
