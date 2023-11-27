// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticsHelper.h"

#include "BlueprintDataDefinitions.h"

bool UStaticsHelper::ActorIsInFront(const AActor* ActorFrom, const FVector& ToLocation)
{
	const FVector DirToTarget = ToLocation - ActorFrom->GetActorLocation();

	const float Dot = FVector::DotProduct(DirToTarget, ActorFrom->GetActorForwardVector());

	return Dot >= 0; // In front of player if Dot Product is > 0, perpendicular when 0 
}

TArray<FBPFriendInfo> UStaticsHelper::SortFriendsList(const TArray<FBPFriendInfo>& FriendsList)
{
	TArray<FBPFriendInfo> SortedFriendsList = FriendsList;

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

	// sort by online

	return SortedFriendsList;
}
