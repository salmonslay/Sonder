// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "CameraSpline.generated.h"

class UBoxComponent;

UCLASS()
class SONDER_API ACameraSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraSpline();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* CameraSpline;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CameraRotation;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CameraEnterBox;

	UFUNCTION()
	void ActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	bool bIsCameraSpline = false;

};
