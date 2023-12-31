// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyJumpTrigger.generated.h"

class APROJCharacter;
class AMovingPlatform;
class AShadowCharacter;
class UBoxComponent;

UCLASS()
class PROJ_API AEnemyJumpTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyJumpTrigger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Bounds;

	UPROPERTY(EditAnywhere)
	UBoxComponent* JumpPoint1;

	UPROPERTY(EditAnywhere)
	UBoxComponent* JumpPoint2;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOverlappingWithMovingPlatform = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOverlappingWithEnemy = false;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void RemoveWaitingEnemy(AShadowCharacter* EnemyToRemove);
	
	UFUNCTION(BlueprintCallable)
	void AddWaitingEnemy(AShadowCharacter* EnemyToAdd);

	UFUNCTION(BlueprintCallable)
	bool IsOverlappingWithEnemies() {	return !WaitingEnemies.IsEmpty();	}

	UFUNCTION(BlueprintCallable)
	bool IsOverlappingWithPlatform() { return bIsOverlappingWithMovingPlatform; }

	UPROPERTY(BlueprintReadWrite)
	AMovingPlatform* OverlappingPlatform = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bTriggerJumpToMovablePlatform = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasPathBetweenJumpPoints = false;

	FVector RequestJumpLocation(const FVector &EnemyLoc, const FVector &CurrentTargetLocation, const bool bIsOnPlatform);
	
	FVector JumpPoint1Loc = FVector::ZeroVector;

	FVector JumpPoint2Loc = FVector::ZeroVector;
	
private:
	
	UPROPERTY(EditAnywhere)
	float EnemyJumpDistance = 200.f;

	UPROPERTY(EditAnywhere)
	float PlatformJumpZOffset = 1000.f;

	UPROPERTY(EditAnywhere)
	float BasicJumpZOffset = 300.f;
	
	TArray<AShadowCharacter*> WaitingEnemies = TArray<AShadowCharacter*>();

	FVector CalculateJumpToPlatform(const FVector& EnemyLocation) const;

	FVector CalculatePointClosetsToTarget(const FVector& EnemyLocation, const FVector& CurrentTargetLocation) const;

	FVector CalculatePointFurthestFromEnemy(const FVector& EnemyLocation) const;

	/** Runs on overlap begin with moving platform, enemies on moving platform are allowed to jump to ground, enemies on ground are allowed to jump on platform*/
	UFUNCTION(BlueprintCallable)
	void AllowJump();

	/** Runs on overlap end with moving platform, enemies on moving platform are denied to jump to ground, enemies on ground are denied to jump on platform*/
	UFUNCTION(BlueprintCallable)
	void DenyJump();

	UFUNCTION(BlueprintCallable)
	bool HasPathBetweenPoints() const;

	bool IsLeveledWithJumpPoints(const FVector &EnemyLoc) const;
	
};
