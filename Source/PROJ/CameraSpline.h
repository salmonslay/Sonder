// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "CameraSpline.generated.h"

UCLASS()
class PROJ_API ACameraSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraSpline();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* CameraSpline;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CameraRotation;

	
	
};
