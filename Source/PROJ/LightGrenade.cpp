// Fill out your copyright notice in the Description page of Project Settings.


#include "LightGrenade.h"
#include "EnhancedInputComponent.h"
#include "PROJCharacter.h"
#include "SoulCharacter.h"
#include "VisualizeTexture.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
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



// Called when the game starts or when spawned
void ALightGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Begin"));

	ExplosionArea->OnComponentBeginOverlap.AddDynamic(this,&ALightGrenade::ActorBeginOverlap);
	

	ExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); 
	
}

void ALightGrenade::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this); 
}



void ALightGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALightGrenade,  bCanThrow);
}


void ALightGrenade::Throw()
{
	ServerRPCThrow();
	
	
}
void ALightGrenade::ServerRPCThrow_Implementation()
{
	if(!this->HasAuthority())
		return;

	MulticastRPCThrow();
}

void ALightGrenade::MulticastRPCThrow_Implementation()
{
	if(bCanThrow)
	{
		TArray<AActor*> FoundCharacter;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASoulCharacter::StaticClass(), FoundCharacter);

		Player = Cast<ASoulCharacter>(FoundCharacter[0]);

		PlayerBase = Cast<APROJCharacter>(Player);

		Controller = PlayerBase->GetInstigatorController();
		
		UE_LOG(LogTemp, Warning, TEXT("Throw Grenade"));
	
		EnableGrenade();

		ExplosionArea->SetWorldLocation(Player->ThrowLoc->GetComponentLocation()); 

		
		
		FVector LandingLoc = ExplosionArea->GetComponentLocation() + (Player->GetActorForwardVector()) - Player->GetActorLocation();

		//ExplosionArea->AddImpulse(FVector(2000.0f,2000.0f,0));
	
		ExplosionArea->SetPhysicsLinearVelocity(LandingLoc*FireSpeed);
		

		bCanThrow = false;

		StartCountdown(10.0f);
	}
}


void ALightGrenade::ActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	if (!HasAuthority()) // Only run on server
		return;
	
		if (OtherActor->ActorHasTag("Enemy"))
		{
			
			StartCountdown(ExplodeTimeFast);
		}else if (OtherActor)
		{
			if (!bIsExploding)
			{
				StartCountdown(ExplodeTimeSlow);
			}
			
		}
	
}

void ALightGrenade::ServerRPCExplosion_Implementation()
{
	// Code here is only run on server, will probably not be changed unless we'll have server specific behaviour 
	
	// Should only run on server 
	if(!this->HasAuthority())
		return; 
	

	bIsExploding = true;
	MulticastRPCExplosion();
}

void ALightGrenade::MulticastRPCExplosion_Implementation()
{
	ExplosionEvent();
	
	TArray<AActor*> OverlapingActors;
	
	ExplosionArea->GetOverlappingActors(OverlapingActors,AActor::StaticClass());
	
	DisableGrenade();


	for (AActor* OverlapingActor : OverlapingActors)
	{
		if(!OverlapingActor->ActorHasTag("Player"))
		{
			UE_LOG(LogTemp, Warning, TEXT("did damage"));
			
			OverlapingActor->TakeDamage(Damage, FDamageEvent(), Controller, this);
		}
		
	}
	
	
}

void ALightGrenade::EnableCanThrow()
{
	bCanThrow = true;
}

void ALightGrenade::DisableGrenade()
{
	//this->SetHidden(true);
	ExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GrenadeMesh->SetVisibility(false);
	bIsExploding = false;
	
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::EnableCanThrow, ThrowCooldown);
	
}

void ALightGrenade::EnableGrenade()
{
	ExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GrenadeMesh->SetVisibility(true);
	//this->SetHidden(false);
	
	
}

void ALightGrenade::StartCountdown(float TimeUntilExplosion)
{
	bIsExploding = true;
	 
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::ServerRPCExplosion, TimeUntilExplosion);
}


