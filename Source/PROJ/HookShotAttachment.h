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

	// Returns the appropriate hook or Soul to target or nullptr if there is none 
	static AActor* GetCurrentTarget(FHitResult& HitResultOut); 

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HookMesh; 

	/** Stores all hooks in the level */
	inline static TArray<AHookShotAttachment*> HookShotAttachmentsInLevel;
	
	/** The current target for the hook shot or nullptr if none */
	inline static AActor* CurrentTarget;

	/** The widget class to display over the current target */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HookIndicatorWidgetClass;
	
	/** The actual widget for the indicator */
	UPROPERTY()
	UUserWidget* IndicatorWidget;

	// Soul is considered as a hook target 
	UPROPERTY()
	AActor* Soul;

	UPROPERTY()
	class APROJCharacter* Robot;

	UPROPERTY()
	class URobotHookingState* HookState;

	UPROPERTY()
	class URobotBaseState* RobotBaseState; 

	inline static APlayerController* RobotPlayerController;

	// The hit result from the latest line trace 
	inline static FHitResult HitResult; 

	/** Returns true if the Hook is a possible target */
	static bool HookCanBeUsed(const AHookShotAttachment* Hook, const AActor* Robot);

	/** Returns true if the hook is visible on screen */
	static bool HookIsOnScreen(const AHookShotAttachment* Hook, const AActor* Robot); 

	/** Returns the closest Hook from the passed array */
	static AHookShotAttachment* GetClosestHook(TArray<AHookShotAttachment*>& Hooks, const AActor* Robot);

	/** Sets current target */
	void SetCurrentHookTarget();

	/** Returns true if Soul is a valid target */
	AActor* GetValidTarget();

	AHookShotAttachment* GetStaticHookToTarget() const;

	/** Sets the widget's location and if it should be active at all */
	void SetIndicatorWidget(); 
	
};
