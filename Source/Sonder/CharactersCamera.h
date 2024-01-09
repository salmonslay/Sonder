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
class SONDER_API ACharactersCamera : public ACameraActor
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

	UFUNCTION(BlueprintCallable)
	bool SetAllowWallsMovement( bool bValue);

private:
	virtual void Tick(float DeltaSeconds) override;
	
	void MoveCamera();

	void RotateCamera() const;

	void MoveWalls(FVector MiddlePoint) const;

	void SetWallCollision() const;
	
	bool bAllowWallsMovement = true;

	bool bAllowMovement;

	UPROPERTY()
	UCameraComponent* CameraComp;

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
