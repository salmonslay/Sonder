// Fill out your copyright notice in the Description page of Project Settings.


#include "HookShotAttachment.h"

#include "PROJCharacter.h"
#include "RobotBaseState.h"
#include "RobotHookingState.h"
#include "RobotStateMachine.h"
#include "SoulCharacter.h"
#include "StaticsHelper.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Engine/UserInterfaceSettings.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHookShotAttachment::AHookShotAttachment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; 

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Hook Mesh"));
	HookMesh->SetupAttachment(RootComponent); 
}

// Called when the game starts or when spawned
void AHookShotAttachment::BeginPlay()
{
	Super::BeginPlay();

	// This class is only relevant for the Robot so disable tick for Soul 
	if(!UGameplayStatics::GetPlayerController(this, 0)->GetPawn()->IsA(ARobotStateMachine::StaticClass()))
		SetActorTickEnabled(false);

	HookShotAttachmentsInLevel.Add(this); 
}

void AHookShotAttachment::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// In Tick because players are loaded in at different times 
	if(!Robot)
	{
		Robot = Cast<ACharacter>(UGameplayStatics::GetActorOfClass(this, ARobotStateMachine::StaticClass()));

		if(Robot)
		{
			RobotPlayerController = Cast<APlayerController>(Cast<APawn>(Robot)->GetController());
			HookState = Robot->FindComponentByClass<URobotHookingState>();
			RobotBaseState = Robot->FindComponentByClass<URobotBaseState>(); 
		}
	}

	if(!Soul)
		Soul = UGameplayStatics::GetActorOfClass(this, ASoulCharacter::StaticClass());

	if(!Robot || !Robot->IsLocallyControlled()) // TODO: also return if hook shot is not unlocked 
		return; 

	SetCurrentHookTarget();

	SetIndicatorWidget();

	// UE_LOG(LogTemp, Warning, TEXT("Current target: %s"), CurrentTarget ? *CurrentTarget->GetActorNameOrLabel() : *FString("nullptr"))
}

void AHookShotAttachment::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// UE_LOG(LogTemp, Warning, TEXT("End play reason: %s"), *UEnum::GetValueAsString(EndPlayReason))

	// Empty the list on end play, we might want to check the EndPlayReasonHere
	// If hooks can be individually destroyed, then we might want to remove a specific hook instead of clearing entire array 
	HookShotAttachmentsInLevel.Empty();

	CurrentTarget = nullptr;
	RobotPlayerController = nullptr; 
}

AActor* AHookShotAttachment::GetCurrentTarget(FHitResult& HitResultOut)
{
	HitResultOut = HitResult; 
	return CurrentTarget; 
}

bool AHookShotAttachment::HookCanBeUsed(const AHookShotAttachment* Hook, const AActor* Robot)
{
	if(!Hook) // No hook 
		return false;

	if(!UStaticsHelper::ActorIsInFront(Robot, Hook)) 
		return false; 

	if(!HookIsOnScreen(Hook, Robot))
		return false;

	return true; // All checks passed, can be used 
}

bool AHookShotAttachment::HookIsOnScreen(const AHookShotAttachment* Hook, const AActor* Robot)
{
	// This function could also be in StaticsHelper class 
	
	if (!RobotPlayerController)
		return false;

	// project hook location to screen
	FVector2D ScreenLocation;
	UGameplayStatics::ProjectWorldToScreen(RobotPlayerController, Hook->GetActorLocation(), ScreenLocation);

	// get the current viewport size
	int32 ViewportSizeX, ViewportSizeY;
	RobotPlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	bool bOnScreen = ScreenLocation.X > 0 && ScreenLocation.X < ViewportSizeX && ScreenLocation.Y > 0 &&
		ScreenLocation.Y < ViewportSizeY; // check if hook is on screen

	// edge case, sometimes the hook is not on screen, but the location is 0,0 
	if (ScreenLocation.X < 1 || ScreenLocation.Y < 1)
		bOnScreen = false;

	return bOnScreen;
}

AHookShotAttachment* AHookShotAttachment::GetClosestHook(TArray<AHookShotAttachment*>& Hooks, const AActor* Robot)
{
	// No hooks 
	if(Hooks.IsEmpty())
		return nullptr;

	// Only one Hook 
	if(Hooks.Num() == 1)
		return Hooks[0];

	// Multiple Hooks, find the closest one 
	AHookShotAttachment* ClosestHook = Hooks[0];
	float ClosestDist = FVector::Dist(ClosestHook->GetActorLocation(), Robot->GetActorLocation()); 

	for(int i = 1; i < Hooks.Num(); i++)
	{
		const float DistToHook = FVector::Dist(Hooks[i]->GetActorLocation(), Robot->GetActorLocation()); 
		if(DistToHook < ClosestDist)
		{
			ClosestDist = DistToHook;
			ClosestHook = Hooks[i]; 
		}
	}

	return ClosestHook; 
}

void AHookShotAttachment::SetCurrentHookTarget()
{
	if(!Robot || !Soul)
	{
		CurrentTarget = nullptr;
		return; 
	}

	CurrentTarget = GetValidTarget(); 
}

AActor* AHookShotAttachment::GetValidTarget()
{
	const FVector StartLoc = Robot->GetActorLocation();
	FVector EndLoc = Soul->GetActorLocation();

	AActor* HookTarget = nullptr;
	bool bTargetingSoul = true;

	// Set new end loc if player does not already have a target 
	if(!HookState->IsTravellingToTarget())
	{
		// If Soul is NOT in front of Robot, only then check if there is a possible hook point to target 
		if(!UStaticsHelper::ActorIsInFront(Robot, Soul))
		{
			bTargetingSoul = false; 
			
			// Set EndLoc to Hook location of there is an eligible hook target 
			HookTarget = GetStaticHookToTarget(); 
			if(HookTarget)
				EndLoc = HookTarget->GetActorLocation();
		} 
	} else if(HookState->IsTargetSoul()) // Update target loc if target is Soul, Soul could've moved 
		EndLoc = Soul->GetActorLocation();

	// Soul not valid and no valid hook 
	if(!bTargetingSoul && !HookTarget)
		return nullptr; 

	FCollisionQueryParams Params;
	Params.AddIgnoredActors( TArray<AActor*>( {Robot, Soul, HookTarget } ) ); // Ignore players and hook 

	const auto CapsuleComp = Robot->GetCapsuleComponent();
	
	// Sweep trace 
	const FCollisionShape CollShape = FCollisionShape::MakeCapsule(CapsuleComp->GetScaledCapsuleRadius() / 2, CapsuleComp->GetScaledCapsuleHalfHeight() / 2);
	GetWorld()->SweepSingleByChannel(HitResult, StartLoc, EndLoc, FQuat::Identity, ECC_Pawn, CollShape, Params); 

	// Return the Hook or Soul if sweep trace did not impact an obstacle 
	if(!HitResult.IsValidBlockingHit())
		return HookTarget ? HookTarget : Soul;

	// Hit an obstacle and is targeting Soul
	if(bTargetingSoul)
	{
		// Get eventual Hook
		HookTarget = GetStaticHookToTarget();
		if(HookTarget) // If there is a valid HookTarget, update the hit result with line trace towards hook instead of towards Soul 
			GetWorld()->SweepSingleByChannel(HitResult, StartLoc, HookTarget->GetActorLocation(), FQuat::Identity, ECC_Pawn, CollShape, Params);

		// Return hook target if there is a clear path to it, otherwise return nullptr - no valid target 
		return HitResult.IsValidBlockingHit() ? nullptr : HookTarget; 
	}

	// No valid target 
	return nullptr; 
}

AHookShotAttachment* AHookShotAttachment::GetStaticHookToTarget() const
{
	if(HookShotAttachmentsInLevel.IsEmpty())
		return nullptr; 
	
	TArray<AHookShotAttachment*> PossibleHooks; 

	for(auto Hook : HookShotAttachmentsInLevel)
	{
		if(HookCanBeUsed(Hook, Robot))
			PossibleHooks.Add(Hook); 
	}
	
	if(PossibleHooks.IsEmpty())
		return nullptr;

	return GetClosestHook(PossibleHooks, Robot); 
}

void AHookShotAttachment::SetIndicatorWidget()
{
	if(!IndicatorWidget)
	{
		if(!Robot)
			return; 
		
		// Create the widget and set hidden as default 
		IndicatorWidget = CreateWidget(RobotPlayerController, HookIndicatorWidgetClass);
		
		if(!IndicatorWidget)
			return;
		
		IndicatorWidget->AddToPlayerScreen();
		IndicatorWidget->SetVisibility(ESlateVisibility::Hidden); 
	}
	
	if (!IndicatorWidget)
		return;
	
	if(!CurrentTarget || HookState->IsTravellingToTarget() || RobotBaseState->IsHookShotOnCooldown())
	{
		IndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
		return; 
	}

	IndicatorWidget->SetVisibility(ESlateVisibility::Visible); 
	
	const APlayerController* PlayerController = RobotPlayerController;

	if (!PlayerController)
		return;

	// get screen bounds 
	int32 ScreenWidth, ScreenHeight;
	PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

	// Source to get DPI Scale: https://forums.unrealengine.com/t/current-dpi-scaling/296699/3 
	const float ScaleDPI = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->
		GetDPIScaleBasedOnSize(FIntPoint(ScreenWidth, ScreenHeight));

	// this makes it moves the same with all resolutions 
	ScreenWidth /= ScaleDPI;

	// calculate new position, where the hook would be on the screen 
	
	FVector2d NewScreenPos;
	UGameplayStatics::ProjectWorldToScreen(PlayerController, CurrentTarget->GetActorLocation(), NewScreenPos);
	NewScreenPos /= ScaleDPI; // also need to adjust the new position to make it match 

	// move the image with the calculated values 
	UWidget* IndicatorImage = IndicatorWidget->GetWidgetFromName(FName("IndicatorImage"));
	IndicatorImage->SetRenderTranslation(NewScreenPos);
}
