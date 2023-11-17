// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Helper functions 
 */
class PROJ_API StaticsHelper
{

public:

	/** Checks if ActorTo is in front of ActorFrom based on ActorFrom's forward vector. Perpendicular counts as in front */
    static bool ActorIsInFront(const AActor* ActorFrom, const AActor* ActorTo) { return ActorIsInFront(ActorFrom, ActorTo->GetActorLocation()); };

	/** Checks if ToLocation is in front of ActorFrom based on ActorFrom's forward vector. Perpendicular counts as in front */
	static bool ActorIsInFront(const AActor* ActorFrom, const FVector& ToLocation);

	// Possible function: static bool IsOnScreen(AActor* Actor, APlayerController* PlayerController); 
	
};
