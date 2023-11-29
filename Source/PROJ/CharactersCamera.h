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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* WallOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* WallTwo;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	ACameraSpline* CurrentCameraSplineClass;

	void SetInterpSpeed(double SwapInterpSpeed);

	void ResetInterpSpeed();

	UPROPERTY()
	double DefaultInterpSpeed;

	void GetPlayers();


private:
	virtual void Tick(float DeltaSeconds) override;

	
	void MoveCamera();

	

	void RotateCamera();

	void MoveWalls(FVector MiddlePoint);

	void SetWallCollision();

	
	

	bool bAllowMovement;

	
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	double InterpSpeedLocation;

	UPROPERTY(EditAnywhere)
	double InterpSpeedRotation;

	
	
	UPROPERTY()
	APROJCharacter* PlayerOne = nullptr;
	
	UPROPERTY()
	APROJCharacter* PlayerTwo = nullptr;

	UPROPERTY()
	USplineComponent* CameraSpline;

	

	
	
};
