// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraSpline.h"
#include "Camera/CameraActor.h"
#include "CharactersCamera.generated.h"

class APROJCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API ACharactersCamera : public ACameraActor
{
	GENERATED_BODY()

	ACharactersCamera();

	

protected:
	virtual void BeginPlay() override;

public:
	ACameraSpline* AssignSpline(ACameraSpline* CameraSplineClass);
	
	FVector TargetLocation;

	UPROPERTY(EditAnywhere)
	ACameraSpline* DefaultCameraSplineClass;

private:
	virtual void Tick(float DeltaSeconds) override;

	
	void MoveCamera();

	void GetPlayers();

	void RotateCamera();

	UPROPERTY()
	ACameraSpline* CurrentCameraSplineClass;

	bool bAllowMovement;

	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	double InterpSpeedLocation;

	UPROPERTY(EditAnywhere)
	double InterpSpeedRotation;
	

	APROJCharacter* PlayerOne = nullptr;
	APROJCharacter* PlayerTwo = nullptr;

	USplineComponent* CameraSpline;

	

	
	
};
