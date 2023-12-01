// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShadowCharacter.h"
#include "ShadowRobotCharacter.generated.h"

UCLASS()
class PROJ_API AShadowRobotCharacter : public AShadowCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShadowRobotCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere)
	class URobotBaseState* RobotBaseState;
	
};
