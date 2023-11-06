// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraSpline.h"
#include "Camera/CameraActor.h"
#include "CharactersCamera.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API ACharactersCamera : public ACameraActor
{
	GENERATED_BODY()

	ACharactersCamera();

	UPROPERTY(EditAnywhere)
	ACameraSpline* CameraSplineClass;

protected:
	virtual void BeginPlay();

private:
	virtual void Tick(float DeltaSeconds) override;
	void MoveCamera();

	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	double InterpSpeed;
	

	ACharacter* PlayerOne;
	ACharacter* PlayerTwo;

	USplineComponent* CameraSpline;
	
};
