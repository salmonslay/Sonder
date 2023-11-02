// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCharState.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_API UPlayerCharState : public UActorComponent
{
	GENERATED_BODY()

public:

	UPlayerCharState(); 

	/** Called when player transitions into this state */
	virtual void Enter() {} 

	/** Called each frame that the state is active */
	virtual void Update(const float DeltaTime) {} 

	/** Called when the state exits to transition to a different state */
	virtual void Exit() {} 

		
};
