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
class PROJ_API UStaticsHelper : public UObject
{
	GENERATED_BODY()

public:
	/** Checks if ActorTo is in front of ActorFrom based on ActorFrom's forward vector. Perpendicular counts as in front */
	static bool ActorIsInFront(const AActor* ActorFrom, const AActor* ActorTo)
	{
		return ActorIsInFront(ActorFrom, ActorTo->GetActorLocation());
	};

	/** Checks if ToLocation is in front of ActorFrom based on ActorFrom's forward vector. Perpendicular counts as in front */
	static bool ActorIsInFront(const AActor* ActorFrom, const FVector& ToLocation);

	// Possible function: static bool IsOnScreen(AActor* Actor, APlayerController* PlayerController); 

	UFUNCTION(BlueprintPure)
	static TArray<FBPFriendInfo> SortFriendsList(const TArray<FBPFriendInfo>& FriendsList);
};
