// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulDashingState.h"

#include "CharacterStateMachine.h"
#include "RobotHookingState.h"
#include "RobotStateMachine.h"
#include "SoulBaseStateNew.h"
#include "SoulCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

		// Dash locally 
		PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
		PlayerOwner->GetCharacterMovement()->AddImpulse(DashForce * DashDir);

		// Dash on server so it does not override the dash 
		ServerRPCDash(DashDir.GetSafeNormal());

		// disable input for the remainder of the dash 
		PlayerOwner->DisableInput(PlayerOwner->GetLocalViewingPlayerController());

		StartLoc = PlayerOwner->GetActorLocation();

		CancelHookShot(); 
	}

}

void USoulDashingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	// Change state/stop dash when velocity is 0 (collided) or travelled max distance 
	if(PlayerOwner->GetCharacterMovement()->Velocity.IsNearlyZero() || FVector::Dist(StartLoc, PlayerOwner->GetActorLocation()) > MaxDashDistance)
		PlayerOwner->SwitchState(Cast<ASoulCharacter>(PlayerOwner)->BaseStateNew);
}

void USoulDashingState::Exit()
{
	Super::Exit();

	if(!PlayerOwner->IsLocallyControlled())
		return;
	
	PlayerOwner->EnableInput(PlayerOwner->GetLocalViewingPlayerController());

	ServerExit(PlayerOwner->GetCharacterMovement()->GetLastInputVector()); 
}

void USoulDashingState::CancelHookShot()
{
	// Cancel Hook shot on client if Soul is played by the server and vice versa 
	if(PlayerOwner->HasAuthority())
		ClientRPC_CancelHookShot();
	else
		ServerRPC_CancelHookShot(); 
}

void USoulDashingState::ClientRPC_CancelHookShot_Implementation()
{
	if(!HookState)
		HookState = UGameplayStatics::GetActorOfClass(this, ARobotStateMachine::StaticClass())->FindComponentByClass<URobotHookingState>(); 

	if(HookState)
		HookState->EndHookShot();
}

void USoulDashingState::ServerRPC_CancelHookShot_Implementation()
{
	if(!HookState)
		HookState = UGameplayStatics::GetActorOfClass(this, ARobotStateMachine::StaticClass())->FindComponentByClass<URobotHookingState>(); 

	if(HookState)
		HookState->EndHookShot();
}

void USoulDashingState::MulticastExit_Implementation()
{
	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Block);
	
	Cast<ASoulCharacter>(PlayerOwner)->OnDashEnd();
}

void USoulDashingState::ServerExit_Implementation(const FVector InputVec)
{
	if(!PlayerOwner->HasAuthority())
		return;

	// Set velocity to max walk speed 
	FVector VelDir = PlayerOwner->GetCharacterMovement()->Velocity;
	VelDir.Z = 0;
	
	if(!PlayerOwner->IsDepthMovementEnabled())
		VelDir.X = 0;
	
	PlayerOwner->GetCharacterMovement()->Velocity = PlayerOwner->GetCharacterMovement()->MaxWalkSpeed * VelDir.GetSafeNormal();

	MulticastExit(); 
}

void USoulDashingState::MulticastRPCDash_Implementation()
{
	// run on both client and server 
	
	Cast<ASoulCharacter>(PlayerOwner)->OnDash();

	// UE_LOG(LogTemp, Warning, TEXT("Multicast dash"))
}

void USoulDashingState::ServerRPCDash_Implementation(const FVector DashDir)
{
	if(!PlayerOwner->HasAuthority())
		return;

	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
	
	PlayerOwner->GetCharacterMovement()->AddImpulse(DashForce * DashDir);

	MulticastRPCDash(); 
}

