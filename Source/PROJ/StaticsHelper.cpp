// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticsHelper.h"

bool StaticsHelper::ActorIsInFront(const AActor* ActorFrom, const FVector& ToLocation)
{
	const FVector DirToTarget = ToLocation - ActorFrom->GetActorLocation();

	const float Dot = FVector::DotProduct(DirToTarget, ActorFrom->GetActorForwardVector());

	return Dot >= 0; // In front of player if Dot Product is > 0, perpendicular when 0 
}
