// Copyright Epic Games, Inc. All Rights Reserved.

#include "PROJCharacter.h"

#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NewPlayerHealthComponent.h"
#include "PlayerBasicAttack.h"
#include "PlayerHealthComponent.h"
#include "ProjPlayerController.h"
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
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	//PlayerHealthComponent = CreateDefaultSubobject<UPlayerHealthComponent>("PlayerHealthCompNew");
	//PlayerHealthComponent->SetIsReplicated(true);

	NewPlayerHealthComponent = CreateDefaultSubobject<UNewPlayerHealthComponent>("NewPlayerHealthComp");
	//NewPlayerHealthComponent->SetIsReplicated(true);

	BasicAttack = CreateDefaultSubobject<UPlayerBasicAttack>(FName("Basic Attack"));
	BasicAttack->SetupAttachment(RootComponent);
	BasicAttack->SetCollisionProfileName("Pawn");
	//CreateComponents(); // tried not doing this, healthicomponents is not initiated correctly
}

void APROJCharacter::CreateComponents()
{
	NewPlayerHealthComponent = CreateDefaultSubobject<UNewPlayerHealthComponent>("NewPlayerHealthComp");
	NewPlayerHealthComponent->SetIsReplicated(true);

	BasicAttack = CreateDefaultSubobject<UPlayerBasicAttack>(FName("Basic Attack"));
	BasicAttack->SetupAttachment(RootComponent);
	BasicAttack->SetCollisionProfileName("Pawn");
}

void APROJCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

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

void APROJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComp = EnhancedInputComponent;
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APROJCharacter::Move);

		// Attack 
		FindComponentByClass<UPlayerBasicAttack>()->SetUpInput(EnhancedInputComponent);
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

	DOREPLIFETIME(APROJCharacter, NewPlayerHealthComponent) // Ex. of how variables are added 
}

void APROJCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
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

float APROJCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); 

	//ensure (HealthComponent != nullptr);
	if (NewPlayerHealthComponent != nullptr)
	{
		DamageApplied = NewPlayerHealthComponent->TakeDamage(DamageApplied);
		UE_LOG(LogTemp, Warning, TEXT("Player %s damaged with %f"), *GetName(), DamageApplied);

	}

	return DamageApplied;
}
