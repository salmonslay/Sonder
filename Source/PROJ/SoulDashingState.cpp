// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulDashingState.h"

#include "CharacterStateMachine.h"
#include "RobotBaseState.h"
#include "RobotHookingState.h"
#include "RobotStateMachine.h"
#include "SoulBaseStateNew.h"
#include "SoulCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void USoulDashingState::Enter()
{
	Super::Enter();

	if(!CharOwner->IsLocallyControlled())
		return;
	
	CharOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
		
	// Set dash direction to input vector if player is moving, otherwise the forward vector 
	FVector DashDir = CharOwner->GetLastMovementInputVector().IsNearlyZero() ? CharOwner->GetActorForwardVector() : CharOwner->GetLastMovementInputVector(); 

	DashDir.Z = 0; // Disable dash in Z axis (up/down)
		
	if(!Cast<APROJCharacter>(CharOwner)->IsDepthMovementEnabled())
		DashDir.X = 0;

	DashDir.Normalize(); 

	// Dash locally 
	CharOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
	CharOwner->GetCharacterMovement()->AddImpulse(DashForce * DashDir);

	// Dash on server so it does not override the dash 
	ServerRPCDash(DashDir);

	// disable input for the remainder of the dash 
	CharOwner->DisableInput(CharOwner->GetLocalViewingPlayerController());

	StartLoc = CharOwner->GetActorLocation();

	CharOwner->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &USoulDashingState::ActorOverlap); 

	CancelHookShot();
}

void USoulDashingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	// Change state/stop dash when velocity is 0 (collided) or travelled max distance 
	if(CharOwner->GetCharacterMovement()->Velocity.IsNearlyZero() || FVector::Dist(StartLoc, CharOwner->GetActorLocation()) > MaxDashDistance)
		Cast<ACharacterStateMachine>(CharOwner)->SwitchState(Cast<ASoulCharacter>(CharOwner)->BaseStateNew);
}

void USoulDashingState::Exit()
{
	Super::Exit();

	if(!CharOwner->IsLocallyControlled())
		return;
	
	CharOwner->EnableInput(CharOwner->GetLocalViewingPlayerController());

	CharOwner->GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &USoulDashingState::ActorOverlap); 

	ServerExit(CharOwner->GetCharacterMovement()->GetLastInputVector()); 
}

void USoulDashingState::CancelHookShot()
{
	// Cancel Hook shot on client if Soul is played by the server and vice versa 
	if(CharOwner->HasAuthority())
		ClientRPC_CancelHookShot();
	else
		ServerRPC_CancelHookShot(); 
}

void USoulDashingState::ActorOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!CharOwner->IsLocallyControlled())
		return;

	// If dashing through Robot, apply buff to Robot 
	if(const auto Robot = Cast<ARobotStateMachine>(OtherActor))
	{
		if(!CharOwner->HasAuthority())
			ServerRPC_RobotBuff(Robot); 
		else
			Robot->FindComponentByClass<URobotBaseState>()->ApplySoulDashBuff(); 
	}
	else 
		ServerRPC_DamageActor(OtherActor); // Otherwise deal damage to overlapping object, needs to be applied on server 
}

void USoulDashingState::ServerRPC_RobotBuff_Implementation(ARobotStateMachine* Robot)
{
	if(!CharOwner->HasAuthority())
		return; 
	
	Robot->FindComponentByClass<URobotBaseState>()->ApplySoulDashBuff();
}

void USoulDashingState::ServerRPC_DamageActor_Implementation(AActor* ActorToDamage)
{
	ActorToDamage->TakeDamage(DashDamageAmount, FDamageEvent(), CharOwner->GetInstigatorController(), CharOwner); 
}

void USoulDashingState::ClientRPC_CancelHookShot_Implementation()
{
	if(!HookState)
		HookState = UGameplayStatics::GetActorOfClass(this, ARobotStateMachine::StaticClass())->FindComponentByClass<URobotHookingState>(); 

	if(HookState)
		HookState->EndHookShot(true);
}

void USoulDashingState::ServerRPC_CancelHookShot_Implementation()
{
	if(!HookState)
		HookState = UGameplayStatics::GetActorOfClass(this, ARobotStateMachine::StaticClass())->FindComponentByClass<URobotHookingState>(); 

	if(HookState)
		HookState->EndHookShot(true);
}

void USoulDashingState::MulticastExit_Implementation()
{
	CharOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Block);
	
	Cast<ASoulCharacter>(CharOwner)->OnDashEnd();
}

void USoulDashingState::ServerExit_Implementation(const FVector InputVec)
{
	if(!CharOwner->HasAuthority())
		return;

	// Set velocity to max walk speed 
	FVector VelDir = CharOwner->GetCharacterMovement()->Velocity;
	VelDir.Z = 0;
	
	if(!Cast<ACharacterStateMachine>(CharOwner)->IsDepthMovementEnabled())
		VelDir.X = 0;

	CharOwner->SetCanBeDamaged(true); 
	
	CharOwner->GetCharacterMovement()->Velocity = CharOwner->GetCharacterMovement()->MaxWalkSpeed * VelDir.GetSafeNormal();

	MulticastExit(); 
}

void USoulDashingState::MulticastRPCDash_Implementation()
{
	// run on both client and server
	
	Cast<ASoulCharacter>(CharOwner)->OnDash();
}

void USoulDashingState::ServerRPCDash_Implementation(const FVector DashDir)
{
	if(!CharOwner->HasAuthority())
		return;

	CharOwner->SetCanBeDamaged(false); // Needs to be set on server 

	CharOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore);
	
	CharOwner->GetCharacterMovement()->AddImpulse(DashForce * DashDir);

	MulticastRPCDash(); 
}

