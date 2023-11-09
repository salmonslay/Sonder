// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class PROJ_API AProjPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	UClass* GetControlledPawnClass() const { return UsedPawnClass; }

	void SetControlledPawnClass(const TSubclassOf<APawn> PawnClass) { UsedPawnClass = PawnClass; }

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;
	
private:

	/** Finds the camera in the level and sets it as view target/the player's camera */
	void SetCamera();

	UPROPERTY()
	ACameraActor* MainCam; 

	inline static int PlayerCount = 0; 

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> RobotPawnClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> BlobPawnClass;
	
	TSubclassOf<APawn> UsedPawnClass;
	
};
