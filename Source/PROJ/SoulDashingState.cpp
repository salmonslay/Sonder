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

	if(!PlayerOwner->IsLocallyControlled())
		return;
	
	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
		
	// Set dash direction to input vector if player is moving, otherwise the forward vector 
	FVector DashDir = PlayerOwner->GetLastMovementInputVector().IsNearlyZero() ? PlayerOwner->GetActorForwardVector() : PlayerOwner->GetLastMovementInputVector(); 

	DashDir.Z = 0; // Disable dash in Z axis (up/down)
		
	if(!PlayerOwner->IsDepthMovementEnabled())
		DashDir.X = 0;

	DashDir.Normalize(); 

	// Dash locally 
	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
	PlayerOwner->GetCharacterMovement()->AddImpulse(DashForce * DashDir);

	// Dash on server so it does not override the dash 
	ServerRPCDash(DashDir);

	// disable input for the remainder of the dash 
	PlayerOwner->DisableInput(PlayerOwner->GetLocalViewingPlayerController());

	StartLoc = PlayerOwner->GetActorLocation();

	PlayerOwner->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &USoulDashingState::ActorOverlap); 

	CancelHookShot();
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

	PlayerOwner->GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &USoulDashingState::ActorOverlap); 

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

void USoulDashingState::ActorOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!PlayerOwner->IsLocallyControlled())
		return;

	// If dashing through Robot, apply buff to Robot 
	if(const auto Robot = Cast<ARobotStateMachine>(OtherActor))
	{
		if(!PlayerOwner->HasAuthority())
			ServerRPC_RobotBuff(Robot); 
		else
			Robot->FindComponentByClass<URobotBaseState>()->ApplySoulDashBuff(); 
	}
	else 
		ServerRPC_DamageActor(OtherActor); // Otherwise deal damage to overlapping object, needs to be applied on server 
}

void USoulDashingState::ServerRPC_RobotBuff_Implementation(ARobotStateMachine* Robot)
{
	if(!PlayerOwner->HasAuthority())
		return; 
	
	Robot->FindComponentByClass<URobotBaseState>()->ApplySoulDashBuff();
}

void USoulDashingState::ServerRPC_DamageActor_Implementation(AActor* ActorToDamage)
{
	ActorToDamage->TakeDamage(DashDamageAmount, FDamageEvent(), PlayerOwner->GetInstigatorController(), PlayerOwner); 
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

	PlayerOwner->SetCanBeDamaged(true); 
	
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

	PlayerOwner->SetCanBeDamaged(false); // Needs to be set on server 

	PlayerOwner->GetCapsuleComponent()->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
	
	PlayerOwner->GetCharacterMovement()->AddImpulse(DashForce * DashDir);

	MulticastRPCDash(); 
}

