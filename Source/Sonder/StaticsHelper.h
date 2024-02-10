// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticsHelper.generated.h"

struct FBPFriendInfo;

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Both,
	Robot,
	Soul,
};

/**
 * Helper functions 
 */
UCLASS()
class SONDER_API UStaticsHelper : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * Checks if ActorTo is in front of ActorFrom based on ActorFrom's forward vector. Perpendicular counts as in front
	 * Set DotCompare (-1 to 1) if other logic is wanted to count something as in front 
	 */
	static bool ActorIsInFront(const AActor* ActorFrom, const AActor* ActorTo, const float DotCompare = 0)
	{
		return ActorIsInFront(ActorFrom, ActorTo->GetActorLocation());
	} 

	/**
	 * Checks if ActorTo is in front of ActorFrom based on ActorFrom's forward vector. Perpendicular counts as in front
	 * Set DotCompare if other logic is wanted to count something as in front 
	 */
	static bool ActorIsInFront(const AActor* ActorFrom, const FVector& ToLocation, const float DotCompare = 0);

	/** Check if the current session is playing locally (2 people on the same device) */
	static bool IsPlayingLocal(const UObject* WorldRefObject); 

	UFUNCTION(BlueprintPure)
	static TArray<FBPFriendInfo> SortFriendsList(const TArray<FBPFriendInfo>& FriendsList);
};
