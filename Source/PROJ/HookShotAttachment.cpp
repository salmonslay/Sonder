// Fill out your copyright notice in the Description page of Project Settings.


#include "HookShotAttachment.h"

#include "PROJCharacter.h"
#include "StaticsHelper.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHookShotAttachment::AHookShotAttachment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // Note Tick is turned off. TODO: We might want Tick for ev. signifiers 

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Hook Mesh"));
	HookMesh->SetupAttachment(RootComponent); 
}

// Called when the game starts or when spawned
void AHookShotAttachment::BeginPlay()
{
	Super::BeginPlay();

	HookShotAttachmentsInLevel.Add(this); 
}

void AHookShotAttachment::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// UE_LOG(LogTemp, Warning, TEXT("End play reason: %s"), *UEnum::GetValueAsString(EndPlayReason))

	// Empty the list on end play, we might want to check the EndPlayReasonHere
	// If hooks can be individually destroyed, then we might want to remove a specific hook instead of clearing entire array 
	HookShotAttachmentsInLevel.Empty(); 
}

AHookShotAttachment* AHookShotAttachment::GetHookToTarget(const APROJCharacter* Robot)
{
	if(HookShotAttachmentsInLevel.IsEmpty() || !Robot)
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

bool AHookShotAttachment::HookCanBeUsed(const AHookShotAttachment* Hook, const APROJCharacter* Robot)
{
	if(!Hook) // No hook 
		return false;

	if(!UStaticsHelper::ActorIsInFront(Robot, Hook)) 
		return false; 

	if(!HookIsOnScreen(Hook, Robot))
		return false;

	return true; // All checks passed, can be used 
}

bool AHookShotAttachment::HookIsOnScreen(const AHookShotAttachment* Hook, const APROJCharacter* Robot)
{
	// This function could also be in StaticsHelper class 
	
	const APlayerController* PlayerController = Cast<APlayerController>(Robot->GetController()); 

	if (!PlayerController)
		return false;

	// project hook location to screen
	FVector2D ScreenLocation;
	UGameplayStatics::ProjectWorldToScreen(PlayerController, Hook->GetActorLocation(), ScreenLocation);

	// get the current viewport size
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	bool bOnScreen = ScreenLocation.X > 0 && ScreenLocation.X < ViewportSizeX && ScreenLocation.Y > 0 &&
		ScreenLocation.Y < ViewportSizeY; // check if hook is on screen

	// edge case, sometimes the hook is not on screen, but the location is 0,0 
	if (ScreenLocation.X < 1 || ScreenLocation.Y < 1)
		bOnScreen = false;

	return bOnScreen;
}

AHookShotAttachment* AHookShotAttachment::GetClosestHook(TArray<AHookShotAttachment*>& Hooks, const APROJCharacter* Robot)
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
