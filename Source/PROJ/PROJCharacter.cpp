// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROJCharacter.h"

#include "BasicAttackComponent.h"
#include "CharactersCamera.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NewPlayerHealthComponent.h"
#include "ProjPlayerController.h"
#include "RobotBaseState.h"
#include "SonderSaveGame.h"
#include "RobotHookingState.h"
#include "ShadowRobotCharacter.h"
#include "SonderGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

APROJCharacter::APROJCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement, auto rotation 
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	NewPlayerHealthComponent = CreateDefaultSubobject<UNewPlayerHealthComponent>("NewPlayerHealthComp");

	AttackComponent = CreateDefaultSubobject<UBasicAttackComponent>(FName("Basic Attack Comp"));
	AttackComponent->SetupAttachment(RootComponent);

	bReplicates = true;
}

void APROJCharacter::SetDepthMovementEnabled(const bool bNewEnable)
{
	bDepthMovementEnabled = bNewEnable;
	GetCharacterMovement()->SetPlaneConstraintEnabled(!bNewEnable);

	GetCharacterMovement()->bOrientRotationToMovement = bNewEnable; // Orient to movement in 3D 
}

void APROJCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if(!Camera)
		Camera = Cast<ACharactersCamera>(UGameplayStatics::GetActorOfClass(this, ACharactersCamera::StaticClass()));

	Camera->GetPlayers();

	//Add Input Mapping Context

	if (!IsLocallyControlled()) return;
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to PlayerController failed"));
	}
}

void APROJCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void APROJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp = EnhancedInputComponent;

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APROJCharacter::CoyoteJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APROJCharacter::Move);

		// Attack 
		EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Triggered, this, &APROJCharacter::DoBasicAttack); 
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void APROJCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APROJCharacter, NewPlayerHealthComponent) 
	DOREPLIFETIME(APROJCharacter, AbilityOne)
	DOREPLIFETIME(APROJCharacter, AbilityTwo)
}

void APROJCharacter::Jump()
{
	Super::Jump();

	bHasJumped = true;

	bCanCoyoteJump = false;

	GetCharacterMovement()->GravityScale = DefaultGravityScale;

	// Needs to be set to true every time player jumps because it is reset when apex event is fired 
	GetCharacterMovement()->bNotifyApex = true;
}

void APROJCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();

	// Increase gravity when reaching jump apex for a "Super Mario Jump" 
	GetCharacterMovement()->GravityScale = GravityScaleWhileFalling;
}

void APROJCharacter::DisableCoyoteJump()
{
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Walking)
		bCanCoyoteJump = false;
}

void APROJCharacter::CoyoteJump()
{
	if (!IsLocallyControlled())
		return;

	if (bCanCoyoteJump && !bHasJumped && GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		ServerRPC_CoyoteJump();

		if (!HasAuthority()) // If player is client, also jump locally to prevent stuttering 
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			GetCharacterMovement()->Velocity.Z = 0;
			Jump();
		}
	}
}

void APROJCharacter::ServerRPC_CoyoteJump_Implementation()
{
	if (!HasAuthority())
		return;

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->Velocity.Z = 0;
	Jump();
}

void APROJCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	// if Robot, check if player is in hook shot, then don't update 
	if(const auto HookState = FindComponentByClass<URobotHookingState>())
	{
		if(HookState->IsHookShotting() && HookState->HasValidTarget())
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			ServerRPC_SetMovementMode(MOVE_Flying);
			return;
		}
	}

	const EMovementMode CurrentMovementMode = GetCharacterMovement()->MovementMode;

	// Reset gravity scale when player becomes grounded 
	if (CurrentMovementMode == MOVE_Walking)
	{
		bHasJumped = false;
		bCanCoyoteJump = true;
		GetCharacterMovement()->GravityScale = DefaultGravityScale;
	}
	else if (CurrentMovementMode == MOVE_Falling && !bHasJumped)
	{
		// Started falling without jumping 
		GetCharacterMovement()->GravityScale = GravityScaleWhileFalling;

		GetWorld()->GetTimerManager().ClearTimer(CoyoteJumpTimer);
		GetWorld()->GetTimerManager().SetTimer(CoyoteJumpTimer, this, &APROJCharacter::DisableCoyoteJump, CoyoteJumpPeriod);
	}
}

void APROJCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
	GetCharacterMovement()->SetPlaneConstraintEnabled(!bDepthMovementEnabled);

	RotatePlayer(GetLastMovementInputVector().Y);

	// Probably not and should not be necessary but attack comp has been in its rebellious phase lately
	if(IsLocallyControlled())
		AttackTimer += DeltaSeconds;
}

bool APROJCharacter::IsAlive()
{
	// TODO: This should be removed. Health is handled by the health component. This was temporary for playtesting arena 
	return GetMesh()->GetRelativeScale3D().GetMax() > 0.5f;
}

void APROJCharacter::SaveGame() const
{
	USonderSaveGame* SaveGameInstance = Cast<USonderSaveGame>(
	UGameplayStatics::LoadGameFromSlot("Sonder", 0));

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Display, TEXT("Creating new save file"))
		SaveGameInstance = Cast<USonderSaveGame>(
			UGameplayStatics::CreateSaveGameObject(USonderSaveGame::StaticClass()));
	}

	if (SaveGameInstance == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("No save game instance found!"));
		return;
	}

	// todo: save stuff here

	UE_LOG(LogTemp, Display, TEXT("Saved game!"))

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "SpaceLeek Save", 0);
}

USonderSaveGame* APROJCharacter::LoadGame() const
{
	USonderSaveGame* SaveGameInstance = Cast<USonderSaveGame>(
	UGameplayStatics::LoadGameFromSlot("Sonder", 0));

	if (SaveGameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No save game instance found! Not loading anything"));
		return nullptr;
	}
	
	return SaveGameInstance;
}

USonderSaveGame* APROJCharacter::GetSaveGameSafe()
{
	USonderSaveGame* SaveGameInstance = Cast<USonderSaveGame>(
		UGameplayStatics::LoadGameFromSlot("Sonder", 0));

	if (SaveGameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No save game instance found!"));

		SaveGameInstance = Cast<
			USonderSaveGame>(UGameplayStatics::CreateSaveGameObject(USonderSaveGame::StaticClass()));
	}

	return SaveGameInstance;
}

void APROJCharacter::SetSaveGame(USonderSaveGame* SaveGame)
{
	UGameplayStatics::SaveGameToSlot(SaveGame, "Sonder", 0);
}

void APROJCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector 
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement, only in depth if enabled 
		if (bDepthMovementEnabled)
			AddMovementInput(ForwardDirection, MovementVector.Y);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APROJCharacter::RotatePlayer(const float HorizontalMovementInput)
{
	// 3D rotation is oriented automatically to movement direction 
	if(bDepthMovementEnabled || !IsLocallyControlled())
		return;
	
	bRotateRight = ShouldRotateRight(HorizontalMovementInput);

	const float DesiredYawRot = GetDesiredYawRot(); 

	FRotator ActorCurrentRot = GetActorRotation();

	// Unreal "clamps" rotation at -180 to 180 which is problematic when trying to lerp to minus values,
	// will never reach so I "undo" their clamp by subtracting 360

	if(bRotateRight && DesiredYawRot < 0 && ActorCurrentRot.Yaw > 0)
		ActorCurrentRot.Yaw -= 360;
	if(!bRotateRight && GetActorForwardVector().X < -0.05 && ActorCurrentRot.Yaw > 0)
		ActorCurrentRot.Yaw -= 360; 

	if(DesiredYawRot == ActorCurrentRot.Yaw)
		return;

	const float NewYawRot = UKismetMathLibrary::FInterpTo_Constant(ActorCurrentRot.Yaw,
		DesiredYawRot, UGameplayStatics::GetWorldDeltaSeconds(this), RotationRateIn2D);

	FRotator NewRot = ActorCurrentRot;
	
	NewRot.Yaw = NewYawRot;
	
	// https://forums.unrealengine.com/t/client-owned-character-rotates-slower-than-listen-server-owned-character/427427
	GetCharacterMovement()->FlushServerMoves();  
	SetActorRotation(NewRot);
	
	ServerRPC_RotatePlayer(NewRot); 
}

bool APROJCharacter::ShouldRotateRight(const float HorizontalMovementInput) const
{
	// Moving right 
	if(HorizontalMovementInput > 0)
		return true;

	// Moving left 
	if(HorizontalMovementInput < 0)
		return false;

	// No input, return current rotation direction 
	return bRotateRight; 
}

float APROJCharacter::GetDesiredYawRot() const
{
	const bool bFacingTowardsCamera = GetActorForwardVector().X < 0.05;

	if(bRotateRight)
		return bFacingTowardsCamera ? -270 : 90; 
	
	return bFacingTowardsCamera ? -90 : -180; 
}

void APROJCharacter::DoBasicAttack()
{
	if(AttackTimer > 0.5f)
		AttackComponent->EnableAttack(); 
	
	if(IsValid(AttackComponent)) {
		if(AttackComponent->Attack()) // Reset timer if could attack 
			AttackTimer = 0; 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No attack component, trying to find one..."))
		AttackComponent = FindComponentByClass<UBasicAttackComponent>();
	}
}

void APROJCharacter::ServerRPC_SetMovementMode_Implementation(const EMovementMode NewMode)
{
	if(!HasAuthority())
		return;

	GetCharacterMovement()->SetMovementMode(NewMode); 
}

void APROJCharacter::ServerRPC_RotatePlayer_Implementation(const FRotator& NewRot)
{
	if(!HasAuthority())
		return; 
	
	SetActorRotation(NewRot); 
}

float APROJCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	// Increase damage by eventual Shadow Robot damage multiplier 
	if(const auto Robot = Cast<AShadowRobotCharacter>(DamageCauser))
		DamageAmount *= Robot->FindComponentByClass<URobotBaseState>()->GetDamageBoostMultiplier();
	
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//ensure (HealthComponent != nullptr);
	if (NewPlayerHealthComponent != nullptr)
	{
		DamageApplied = NewPlayerHealthComponent->TakeDamage(DamageApplied);

		if(DamageApplied >= NewPlayerHealthComponent->GetHealth() && NewPlayerHealthComponent->GetHealth() > 0)
		{
			Cast<USonderGameInstance>(GetGameInstance())->AddToLog("Player " + GetName() + " died from " + DamageCauser->GetName());
		}
	}

	return DamageApplied;
}
