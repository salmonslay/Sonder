// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyJumpTrigger.generated.h"

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

	UPROPERTY(BlueprintReadWrite)
	AMovingPlatform* OverlappingPlatform = nullptr;
	
private:
	UPROPERTY(EditAnywhere)
	float EnemyJumpOffset = 50.f;
		
	TArray<AShadowCharacter*> WaitingEnemies = TArray<AShadowCharacter*>();

	FVector CalculateJumpEndPoint(const FVector& EnemyLocation);

	UFUNCTION(BlueprintCallable)
	void AllowJumpToPlatform();

	UFUNCTION(BlueprintCallable)
	void DenyJumpToPlatform();
	
};
