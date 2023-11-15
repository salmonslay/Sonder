// Fill out your copyright notice in the Description page of Project Settings.


#include "LightGrenade.h"
#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "SoulCharacter.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALightGrenade::ALightGrenade()
{

	ExplosionArea = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionArea"));
	ExplosionArea->SetupAttachment(RootComponent);
	
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetupAttachment(ExplosionArea);

	
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ALightGrenade::SetUpInput(UEnhancedInputComponent* InputComp)
{
	InputComp->BindAction(AttackInputAction, ETriggerEvent::Started, this, &ALightGrenade::Throw);
}


// Called when the game starts or when spawned
void ALightGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Begin"));

	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::Throw, 5.0f);

	Player = Cast<ASoulCharacter>(GetOwner());

	ExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); 
	
}

void ALightGrenade::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}

void ALightGrenade::Throw()
{
	/*
	if(!bCanThrow || !Player->IsLocallyControlled())
	{
		return;
	}
		

	
	

	// Run server function which will update each client and itself
	EnableGrenade();
	StartCountdown();
	*/

	UE_LOG(LogTemp, Warning, TEXT("Throw"));

	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::StartCountdown, 1.0f);

	//FVector LandingLoc = ExplosionArea->GetComponentLocation() + FVector(0,0,1000)/* - Player->FireLoc->GetComponentLocation()*/;
	
	//ExplosionArea->SetPhysicsLinearVelocity(LandingLoc*FireSpeed);
	
	
	
}

void ALightGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALightGrenade,  bCanThrow);
}


void ALightGrenade::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority()) // Only run on server
		return;

	if (OtherActor->ActorHasTag("Enemy"))
	{
		ServerRPCExplosion();
	}else if (!OtherActor->ActorHasTag("Player"))
	{
		if (!bIsExploding)
		{
			StartCountdown();
		}
		
	}
}

void ALightGrenade::ServerRPCExplosion_Implementation()
{
	// Code here is only run on server, will probably not be changed unless we'll have server specific behaviour 
	
	// Should only run on server 
	if(!this->HasAuthority())
		return; 

	UE_LOG(LogTemp, Warning, TEXT("Server attack"));

	bIsExploding = true;
	MulticastRPCExplosion();
}

void ALightGrenade::MulticastRPCExplosion_Implementation()
{
	TArray<AActor*> OverlapingActors;
	
	ExplosionArea->GetOverlappingActors(OverlapingActors,AActor::StaticClass());

	UE_LOG(LogTemp, Warning, TEXT("attack"));


	for (AActor* OverlapingActor : OverlapingActors)
	{
		if(!OverlapingActor->ActorHasTag("Player"))
		{
			UE_LOG(LogTemp, Warning, TEXT("did damage"));
			//OverlapingActor->TakeDamage(Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), this);
		}
		
	}
}

void ALightGrenade::EnableCanThrow()
{
	bCanThrow = true;
}

void ALightGrenade::DisableGrenade()
{
	this->SetHidden(true);
	ExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	bIsExploding = false;
	
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::EnableCanThrow, ThrowCooldown);
	
}

void ALightGrenade::EnableGrenade()
{
	ExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	this->SetHidden(false);
	
}

void ALightGrenade::StartCountdown()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Countdown"));
	FTimerHandle TimerHandle; 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::ServerRPCExplosion, ExplodeTime);
}


