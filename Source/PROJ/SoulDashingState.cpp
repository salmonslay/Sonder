// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulDashingState.h"

#include "CharacterStateMachine.h"
#include "RobotBaseState.h"
#include "RobotHookingState.h"
#include "RobotStateMachine.h"
#include "ShadowCharacter.h"
#include "ShadowRobotCharacter.h"
#include "ShadowSoulCharacter.h"
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
	
	CapsuleComponent->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
		
	// Set dash direction to input vector if player is moving, otherwise the forward vector 
	FVector DashDir = CharOwner->GetLastMovementInputVector().IsNearlyZero() ? CharOwner->GetActorForwardVector() : CharOwner->GetLastMovementInputVector(); 

	DashDir.Z = 0; // Disable dash in Z axis (up/down)

	if(!CharOwner->IsPlayerControlled() || (CharOwner->IsPlayerControlled() && !Cast<APROJCharacter>(CharOwner)->IsDepthMovementEnabled()))
		DashDir.X = 0; 
	
	DashDir.Normalize(); 

	// Dash locally 
	CapsuleComponent->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore); 
	MovementComponent->AddImpulse(DashForce * DashDir);

	// Dash on server so it does not override the dash 
	ServerRPCDash(DashDir);

	// disable input for the remainder of the dash 
	CharOwner->DisableInput(CharOwner->GetLocalViewingPlayerController());

	StartLoc = CharOwner->GetActorLocation();

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &USoulDashingState::ActorOverlap);

	// Deal damage or buff appropriate character that are already colliding 
	TArray<AActor*> OverlappingActors;
	CapsuleComponent->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for(const auto OverlapActor : OverlappingActors)
		DashOverlap(OverlapActor); 

	if(CharOwner->IsPlayerControlled())
		CancelHookShot();
}

void USoulDashingState::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = CharOwner->GetCharacterMovement();
	CapsuleComponent = CharOwner->GetCapsuleComponent();

	PlayerOwner = Cast<ASoulCharacter>(CharOwner); 
	ShadowOwner = Cast<AShadowSoulCharacter>(CharOwner); 
}

void USoulDashingState::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);

	// Change state/stop dash when velocity is 0 (collided) or travelled max distance 
	if(MovementComponent->Velocity.IsNearlyZero() || FVector::Dist(StartLoc, CharOwner->GetActorLocation()) > MaxDashDistance)
	{
		if(CharOwner->IsPlayerControlled()) // Player 
			PlayerOwner->SwitchState(PlayerOwner->BaseStateNew);
		else // AI 
			ShadowOwner->SwitchState(ShadowOwner->SoulBaseState); 
	}
}

void USoulDashingState::Exit()
{
	Super::Exit();

	if(!CharOwner->IsLocallyControlled())
		return;
	
	CharOwner->EnableInput(CharOwner->GetLocalViewingPlayerController());

	CapsuleComponent->OnComponentBeginOverlap.RemoveDynamic(this, &USoulDashingState::ActorOverlap); 

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

	DashOverlap(OtherActor); 
}

void USoulDashingState::DashOverlap(AActor* OtherActor)
{
	// Player dashing through player or AI dashing through AI 
	if((CharOwner->IsPlayerControlled() && OtherActor->IsA(ARobotStateMachine::StaticClass())) ||
		(!CharOwner->IsPlayerControlled() && OtherActor->IsA(AShadowRobotCharacter::StaticClass())))
	{
		if(const auto RobotState = OtherActor->FindComponentByClass<URobotBaseState>())
		{
			if(!CharOwner->HasAuthority())
				ServerRPC_RobotBuff(RobotState); 
			else
				RobotState->ApplySoulDashBuff(); 
		}
		return; 
	}
		
	ServerRPC_DamageActor(OtherActor); // Otherwise deal damage to overlapping object, needs to be applied on server 
}

void USoulDashingState::ServerRPC_RobotBuff_Implementation(URobotBaseState* RobotBaseState)
{
	if(!CharOwner->HasAuthority())
		return; 
	
	RobotBaseState->ApplySoulDashBuff();
}

void USoulDashingState::ServerRPC_DamageActor_Implementation(AActor* ActorToDamage)
{
	ActorToDamage->TakeDamage(DashDamageAmount, FDamageEvent(), CharOwner->GetInstigatorController(), CharOwner); 
}

void USoulDashingState::ClientRPC_CancelHookShot_Implementation()
{
	EndHookShot(); 
}

void USoulDashingState::ServerRPC_CancelHookShot_Implementation()
{
	EndHookShot(); 
}

void USoulDashingState::EndHookShot()
{
	if(!HookState)
		if(const auto Robot = UGameplayStatics::GetActorOfClass(this, ARobotStateMachine::StaticClass()))
			HookState = Robot->FindComponentByClass<URobotHookingState>();

	if(HookState)
		HookState->EndHookShot(true);
}

void USoulDashingState::MulticastExit_Implementation()
{
	CapsuleComponent->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Block);

	if(CharOwner->IsPlayerControlled()) 
		PlayerOwner->OnDashEnd();
	else
		ShadowOwner->OnDashEnd(); 
}

void USoulDashingState::ServerExit_Implementation(const FVector InputVec)
{
	if(!CharOwner->HasAuthority())
		return;

	// Set velocity to max walk speed 
	FVector VelDir = MovementComponent->Velocity;
	VelDir.Z = 0;
	
	if(CharOwner->IsPlayerControlled() && !PlayerOwner->IsDepthMovementEnabled())
		VelDir.X = 0;

	CharOwner->SetCanBeDamaged(true); 
	
	MovementComponent->Velocity = MovementComponent->MaxWalkSpeed * VelDir.GetSafeNormal();

	MulticastExit(); 
}

void USoulDashingState::MulticastRPCDash_Implementation()
{
	if(CharOwner->IsPlayerControlled())
		PlayerOwner->OnDash();
	else
		ShadowOwner->OnDash(); 
}

void USoulDashingState::ServerRPCDash_Implementation(const FVector DashDir)
{
	if(!CharOwner->HasAuthority())
		return;

	CharOwner->SetCanBeDamaged(false); // Needs to be set on server 

	CapsuleComponent->SetCollisionResponseToChannel(DashBarCollisionChannel, ECR_Ignore);
	
	MovementComponent->AddImpulse(DashForce * DashDir);

	MulticastRPCDash(); 
}
