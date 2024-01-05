// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "PROJCharacter.h"
#include "SonderGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

ACutsceneManager::ACutsceneManager()
{
	bReplicates = true; 
}

void ACutsceneManager::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if(!bPlayOnOverlap || !OtherActor->IsA(APROJCharacter::StaticClass()))
		return; 

	DisablePlayerInput(); 
	PlayCutscene(); 
}

void ACutsceneManager::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<USonderGameInstance>(GetGameInstance()); 

	// will only be 1 if playing online, 2 if playing local 
	for(int i = 0; i < UGameplayStatics::GetNumLocalPlayerControllers(this); i++)
		PlayerControllers.Add(UGameplayStatics::GetPlayerController(this, i)); 

	if(HasAuthority())
		GetWorldTimerManager().SetTimerForNextTick(this, &ACutsceneManager::BindSkipCutsceneButton); 

	// Do not play cutscene if the level was loaded from itself/reset (as in died in arenas) 
	if(bAutoPlay && !GameInstance->LevelWasReset())
	{
		if(AutoPlayBlackScreenWidget)
		{
			// widget only needs to be created for ctrl 0 (only one online and local doesn't matter who gets the widget)
			AutoPlayWidget = CreateWidget(PlayerControllers[0], AutoPlayBlackScreenWidget);

			AutoPlayWidget->AddToPlayerScreen(); 
		}
	
		// Disable input immediately so player cannot move during level load (delayed because race conditions)
		GetWorldTimerManager().SetTimerForNextTick(this, &ACutsceneManager::DisablePlayerInput); 
		GetWorldTimerManager().SetTimerForNextTick(this, &ACutsceneManager::RemoveHUD); 

		// Trigger play on server so it syncs 
		if(HasAuthority())
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ACutsceneManager::ServerRPC_PlayCutscene, AutoPlayDelayTime);
		}
	}
}

void ACutsceneManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CutscenesPlayingCounter = 0; 

	GetWorldTimerManager().ClearAllTimersForObject(this); 
}

void ACutsceneManager::PlayCutscene()
{
	if(bHasPlayed)
	{
		EnablePlayerInput();
		return; 
	}
	
	if(!Sequencer)
	{
		EnablePlayerInput(); 
		UE_LOG(LogTemp, Warning, TEXT("No level sequencer assigned to cutscene manager"))
		return;
	}

	DisablePlayerInput(); 

	if(AutoPlayWidget)
		AutoPlayWidget->RemoveFromParent();
	
	RemoveHUD();

	if(bHidePlayersDuringCutscene)
		TogglePlayerVisibility(false);  
	
	ALevelSequenceActor* LevelSequenceActor; // Out actor, the actor for the level sequence player
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, Sequencer, FMovieSceneSequencePlaybackSettings(), LevelSequenceActor);
	
	LevelSequencePlayer->Play();

	// Call stop playing when cutscene finished
	FTimerHandle TimerHandle; 
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACutsceneManager::StopCutscene, LevelSequencePlayer->GetDuration().AsSeconds());

	CutscenesPlayingCounter++;
	bHasPlayed = true; 
}

void ACutsceneManager::ServerRPC_PlayCutscene_Implementation()
{
	if(!HasAuthority())
		return;

	MulticastRPC_PlayCutscene(); 
}

void ACutsceneManager::MulticastRPC_PlayCutscene_Implementation()
{
	PlayCutscene(); 
}

void ACutsceneManager::DisablePlayerInput() const
{
	if(PlayerControllers.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No player ctrl in cutscene"))
		return; 
	}

	for(const auto Controller : PlayerControllers)
	{
		if(!IsValid(Controller))
			return; 
		
		// Set the action mapping to Cutscene action mapping 
		if (const auto InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
		{
			// Switches the action mapping 
			InputSystem->RemoveMappingContext(DefaultInputMap); 
			InputSystem->AddMappingContext(CutsceneInputMap, 1);
		} 	
	}
}

void ACutsceneManager::EnablePlayerInput() const
{
	for(const auto Controller : PlayerControllers)
	{
		if(!IsValid(Controller))
			return; 
		
		// Set the action mapping to the default action mapping 
		if (const auto InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
		{
			// Switches the action mapping 
			InputSystem->RemoveMappingContext(CutsceneInputMap); 
			InputSystem->AddMappingContext(DefaultInputMap, 1);
		}
	}
}

void ACutsceneManager::BindSkipCutsceneButton()
{
	for(const auto Controller : PlayerControllers)
	{
		if(!IsValid(Controller))
			return; 
	
		// Bind skip cutscene button (is "activated" first when cutscene plays)
		if(const auto InputComp = Cast<UEnhancedInputComponent>(Controller->InputComponent))
			InputComp->BindAction(SkipInputAction, ETriggerEvent::Started, this, &ACutsceneManager::ServerRPC_StopCutscene);
		
	}
}

void ACutsceneManager::StopCutscene()
{
	if(!IsCutscenePlaying() || !LevelSequencePlayer) // Not playing, cant stop cutscene so do nothing 
		return;
	
	EnablePlayerInput();

	if(bHidePlayersDuringCutscene)
		TogglePlayerVisibility(true);  

	if(LevelSequencePlayer)
		LevelSequencePlayer->Stop();

	CutscenesPlayingCounter--;

	OnCutsceneEnd(LevelToLoadOnCutsceneEnd != ESonderLevel::None); 

	if(LevelToLoadOnCutsceneEnd != ESonderLevel::None && HasAuthority())
		GameInstance->LoadNewLevel(LevelToLoadOnCutsceneEnd);
	else 
		ShowHud(); 
}

void ACutsceneManager::TogglePlayerVisibility(const bool bVisible) const
{
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(this, APROJCharacter::StaticClass(), Players);

	for(const auto Player : Players)
		Player->SetActorHiddenInGame(!bVisible); 
}

void ACutsceneManager::MulticastRPC_StopCutscene_Implementation()
{
	StopCutscene(); 
}

void ACutsceneManager::ServerRPC_StopCutscene_Implementation()
{
	if(!HasAuthority())
		return;

	MulticastRPC_StopCutscene();
}

void ACutsceneManager::RemoveHUD()
{
	WidgetsHidden.Empty(); 

	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), WidgetsHidden, WidgetsToHide);

	for(const auto Widget : WidgetsHidden)
	{
		if(Widget != AutoPlayWidget)
			Widget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ACutsceneManager::ShowHud()
{
	for(const auto Widget : WidgetsHidden)
		Widget->SetVisibility(ESlateVisibility::Visible); 
}
