// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PROJCharacter.generated.h"

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
	void SetDepthMovementEnabled(const bool bNewEnable) { bDepthMovementEnabled = bNewEnable; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageToPlayer = 0.f;
	
	UPROPERTY(EditAnywhere, Category=Health, BlueprintReadOnly)
	class UPlayerHealthComponent* HealthComponent = nullptr;

#pragma region Events 
	
	// Components seem to not be able to create events (easily), which is why the event is declared here 
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
	virtual void BeginPlay();

private:

	/** Determines if player can move in both axes */
	bool bDepthMovementEnabled = false;

	UPROPERTY()
	class UPlayerBasicAttack* BasicAttack;
	
	void CreateComponents(); 
	
};

