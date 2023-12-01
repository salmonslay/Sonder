// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "PROJCharacter.h"
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
	
	// if(GetNetMode() == ENetMode::NM_Client) // Client has no owning connection so cant call perform RPCs 
	// 	SetOwner(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)); 

	// TODO: Simulates cutscene, call Play from fade widget instead? Perhaps not necessary if all levels have the same intro length 
	if(bAutoPlay)
	{
		// Disable input immediately so player cannot move during level load (delayed because race conditions)
		GetWorldTimerManager().SetTimerForNextTick(this, &ACutsceneManager::DisablePlayerInput); 

		// Trigger play on server so it syncs 
		if(HasAuthority())
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ACutsceneManager::ServerRPC_PlayCutscene, 1.5f);
		}
	}
}

void ACutsceneManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	switch (EndPlayReason)
	{
	case EEndPlayReason::Quit: 
	case EEndPlayReason::EndPlayInEditor: 
		CutscenesPlayingCounter = 0;
		GetWorldTimerManager().ClearAllTimersForObject(this);
		break;
		
	default: ; // Do nothing 
	}
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
	// TODO: Cant skip on client rn 
	
	// Bind skip cutscene button (is "activated" first when cutscene plays)
	if(const auto InputComp = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		InputComp->BindAction(SkipInputAction, ETriggerEvent::Started, this, &ACutsceneManager::ServerRPC_StopCutscene); 
}

void ACutsceneManager::StopCutscene()
{
	if(!IsCutscenePlaying()) // Not playing, cant stop cutscene so do nothing 
		return;
	
	EnablePlayerInput(); 

	LevelSequencePlayer->Stop();

	CutscenesPlayingCounter--;

	if(HasAuthority() && !LevelToLoadOnCutsceneEnd.IsNone())
		GetWorld()->ServerTravel("/Game/Maps/" + LevelToLoadOnCutsceneEnd.ToString());

	if(!IsCutscenePlaying())
		Destroy(); // TODO: call on server (client has RPC issues)
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
