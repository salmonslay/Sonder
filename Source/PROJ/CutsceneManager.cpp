// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "PROJCharacter.h"
#include "Blueprint/UserWidget.h"
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

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	GetWorldTimerManager().SetTimerForNextTick(this, &ACutsceneManager::BindSkipCutsceneButton); 

	if(bAutoPlay)
	{
		// Disable input immediately so player cannot move during level load (delayed because race conditions)
		GetWorldTimerManager().SetTimerForNextTick(this, &ACutsceneManager::DisablePlayerInput); 

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

	GetWorldTimerManager().ClearAllTimersForObject(this); // TODO: Will need switch if object is destroyed 
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
	// Set the action mapping to Cutscene action mapping 
	if (const auto InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		// Switches the action mapping 
		InputSystem->RemoveMappingContext(DefaultInputMap); 
		InputSystem->AddMappingContext(CutsceneInputMap, 1);
	} 
}

void ACutsceneManager::EnablePlayerInput() const
{
	// Set the action mapping to the default action mapping 
	if (const auto InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		// Switches the action mapping 
		InputSystem->RemoveMappingContext(CutsceneInputMap); 
		InputSystem->AddMappingContext(DefaultInputMap, 1);
	}
}

void ACutsceneManager::BindSkipCutsceneButton()
{
	// TODO: Cant skip on client rn. I think it needs to be passed through the player controller, i.e make a server rpc
	// TODO: function on the player controller class which is called when pressing skip https://cedric-neukirchen.net/docs/multiplayer-compendium/ownership/ 
	
	// Bind skip cutscene button (is "activated" first when cutscene plays)
	if(const auto InputComp = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		InputComp->BindAction(SkipInputAction, ETriggerEvent::Started, this, &ACutsceneManager::ServerRPC_StopCutscene); 
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

	if(!LevelToLoadOnCutsceneEnd.IsNone())
	{
		if(WidgetForLoadingNewMap)
		{
			const auto Widget = CreateWidget(PlayerController, WidgetForLoadingNewMap);

			Widget->AddToPlayerScreen(); 
		}	

		if(HasAuthority())
			GetWorld()->ServerTravel("/Game/Maps/" + LevelToLoadOnCutsceneEnd.ToString());
	}

	// if(!IsCutscenePlaying())
	// 	Destroy(); // TODO: Destroy? 
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
	// TODO: Everything 
}
