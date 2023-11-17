// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookShotAttachment.generated.h"

UCLASS()
class PROJ_API AHookShotAttachment : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	AHookShotAttachment();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	// Returns the appropriate hook to target or nullptr if there is none 
	static AHookShotAttachment* GetHookToTarget(const class APROJCharacter* Robot); 
	
private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HookMesh; 

	/** Stores all hooks in the level */
	inline static TArray<AHookShotAttachment*> HookShotAttachmentsInLevel; 

	/** Returns true if the Hook is a possible target */
	static bool HookCanBeUsed(const AHookShotAttachment* Hook, const APROJCharacter* Robot);

	/** Returns true if the hook is visible on screen */
	static bool HookIsOnScreen(const AHookShotAttachment* Hook, const APROJCharacter* Robot); 

	/** Returns the closest Hook from the passed array */
	static AHookShotAttachment* GetClosestHook(TArray<AHookShotAttachment*>& Hooks, const APROJCharacter* Robot); 
	
};
