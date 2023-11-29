// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PROJCharacter.generated.h"

class AProjPlayerController;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APROJCharacter : public ACharacter
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	APROJCharacter();

	/** Toggles depth movement */
	UFUNCTION(BlueprintCallable)
	void SetDepthMovementEnabled(const bool bNewEnable);

	/** Returns true if player can traverse in the depth axis */
	bool IsDepthMovementEnabled() const { return bDepthMovementEnabled; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageToPlayer = 0.f;

	UEnhancedInputComponent* GetInputComponent() const { return EnhancedInputComp; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Health, Replicated)
	class UNewPlayerHealthComponent* NewPlayerHealthComponent = nullptr;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FTransform GetSpawnTransform() const { return SpawnTransform; }

	UFUNCTION(BlueprintCallable)
	void SetSpawnTransform(const FTransform& NewTransform) { SpawnTransform = NewTransform; }

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere)
	class UPlayerBasicAttack* BasicAttack;

	/** Overridden to enable notify jump apex */
	virtual void Jump() override;

	/** Changes gravity scale when at jump peak to fall faster */
	virtual void NotifyJumpApex() override;

	// Checks if it is a Arena to set Death Mode. Will be deprecated when respawn system is reworked
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsArena = false;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	virtual void Tick(float DeltaSeconds) override;

	// Bools controlling players ability to use abilities
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool AbilityOne = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool AbilityTwo = false;

	UFUNCTION(BlueprintPure)
	bool IsAlive();
#pragma region Events

	// Components seem to not be able to create events (easily), which is why most events are declared here 

	/** Event called when player performs a basic attack */
	UFUNCTION(BlueprintImplementableEvent)
	void OnBasicAttack();

	/** Event called when player takes damage, with the damage taken passed as an argument */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageTaken(float DamageAmount);

	/** Event called when player dies */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerDied();

#pragma endregion

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Determines if player can move in both axes */
	bool bDepthMovementEnabled = false;

	void CreateComponents();

	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComp;

	FTransform SpawnTransform;

	/** Set in BeginPlay. Uses the existing rotation rate variable exposed to BluePrints */
	FRotator RotationRateIn2DView = FRotator(0, 1000.f, 0);

	/** How fast the player should rotate in 3D view. Negative value means instant */
	UPROPERTY(EditAnywhere)
	float RotationRateIn3DView = 500.f;

	/** Grounded gravity scale and when jumping upwards */
	UPROPERTY(EditAnywhere)
	float DefaultGravityScale = 1.75f;

	UPROPERTY(EditAnywhere)
	float GravityScaleWhileFalling = 3.f;

	// If player can Coyote jump (has been in the air short enough time and )
	bool bCanCoyoteJump = true;

	bool bHasJumped = false;

	UPROPERTY(EditAnywhere)
	float CoyoteJumpPeriod = 0.1f;

	void DisableCoyoteJump();

	FTimerHandle CoyoteJumpTimer;

	void CoyoteJump();

	UFUNCTION(Server, Reliable)
	void ServerRPC_CoyoteJump();
};
