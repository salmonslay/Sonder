// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulDashingState.h"

#include "CharacterStateMachine.h"
#include "SoulBaseState.h"
#include "SoulCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void USoulDashingState::Enter()
{
	Super::Enter(); 

	if(PlayerOwner->IsLocallyControlled()) 
	{
		PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
		
		// Set dash direction to input vector if player is moving, otherwise the forward vector 
		FVector DashDir = PlayerOwner->GetLastMovementInputVector().IsNearlyZero() ? PlayerOwner->GetActorForwardVector() : PlayerOwner->GetLastMovementInputVector(); 

		DashDir.Z = 0; // Disable dash in Z axis (up/down)
		
		if(!PlayerOwner->IsDepthMovementEnabled())
			DashDir.X = 0; 
		
		ServerRPCDash(DashDir.GetSafeNormal());

		// disable input for the remainder of the dash 
		PlayerOwner->DisableInput(PlayerOwner->GetLocalViewingPlayerController());
		
		TempTimer = 0; 
	}

}

void USoulDashingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	TempTimer += DeltaTime;

	if(TempTimer > 0.5f)
		PlayerOwner->SwitchState(Cast<ASoulCharacter>(PlayerOwner)->BaseState); 
}

void USoulDashingState::Exit()
{
	Super::Exit();

	if(!PlayerOwner->IsLocallyControlled())
		return;
	
	PlayerOwner->EnableInput(PlayerOwner->GetLocalViewingPlayerController());
	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Block); 

	ServerExit(); 
}

void USoulDashingState::ServerExit_Implementation()
{
	if(!PlayerOwner->HasAuthority())
		return;

	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Block); 
}

void USoulDashingState::MulticastRPCDash_Implementation()
{
	if(!PlayerOwner->IsLocallyControlled())
		return;

	// might not be needed
	
	UE_LOG(LogTemp, Warning, TEXT("Multicast dash"))
}

void USoulDashingState::ServerRPCDash_Implementation(const FVector DashDir)
{
	if(!PlayerOwner->HasAuthority())
		return;

	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
	
	PlayerOwner->GetCharacterMovement()->AddImpulse(DashForce * DashDir);

	MulticastRPCDash(); 
}
