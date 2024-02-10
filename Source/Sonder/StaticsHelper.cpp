// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticsHelper.h"

#include "BlueprintDataDefinitions.h"
#include "Kismet/GameplayStatics.h"

bool UStaticsHelper::ActorIsInFront(const AActor* ActorFrom, const FVector& ToLocation, const float DotCompare)
{
	FVector DirToTarget = ToLocation - ActorFrom->GetActorLocation();

	if(DotCompare != 0) // no need to normalize if dot is zero since then we only need to see if the dot is pos or neg 
		DirToTarget.Normalize(); 

	const float Dot = FVector::DotProduct(DirToTarget, ActorFrom->GetActorForwardVector());

	return Dot >= DotCompare; // In front of player if Dot Product is > 0, perpendicular when 0 
}

bool UStaticsHelper::IsPlayingLocal(const UObject* WorldRefObject)
{
	return UGameplayStatics::GetNumLocalPlayerControllers(WorldRefObject) > 1; 
}

TArray<FBPFriendInfo> UStaticsHelper::SortFriendsList(const TArray<FBPFriendInfo>& FriendsList)
{
	TArray<FBPFriendInfo> SortedFriendsList = FriendsList;

	// sort alphabetically
	SortedFriendsList.StableSort([](const FBPFriendInfo& A, const FBPFriendInfo& B)
	{
		return A.DisplayName < B.DisplayName;
	});

	// sort by playing same game
	SortedFriendsList.StableSort([](const FBPFriendInfo& A, const FBPFriendInfo& B)
	{
		return A.bIsPlayingSameGame > B.bIsPlayingSameGame;
	});

	// sort not offline -> not online (we count stuff like away too, but not as high as online)
	SortedFriendsList.StableSort([](const FBPFriendInfo& A, const FBPFriendInfo& B)
	{
		return A.OnlineState != EBPOnlinePresenceState::Offline && B.OnlineState != EBPOnlinePresenceState::Online;
	});

	return SortedFriendsList;
}
