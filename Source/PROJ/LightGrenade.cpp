// Fill out your copyright notice in the Description page of Project Settings.


#include "LightGrenade.h"

#include "EnemyCharacter.h"
#include "EnhancedInputComponent.h"
#include "PressurePlateBase.h"
#include "PROJCharacter.h"
#include "SonderGameState.h"
#include "SoulCharacter.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALightGrenade::ALightGrenade()
{

	ExplosionArea = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionArea"));
	ExplosionArea->SetupAttachment(RootComponent);
	
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetupAttachment(ExplosionArea);

	CollisionArea = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionArea"));
	CollisionArea->SetupAttachment(ExplosionArea);
	
	PulseExplosionArea = CreateDefaultSubobject<USphereComponent>(TEXT("PulseExplosionArea"));
	PulseExplosionArea->SetupAttachment(ExplosionArea);
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALightGrenade::BeginPlay()
{
	Super::BeginPlay();

	CollisionArea->OnComponentBeginOverlap.AddDynamic(this,&ALightGrenade::ActorBeginOverlap);
	CollisionArea->OnComponentEndOverlap.AddDynamic(this,&ALightGrenade::OverlapEnd);
	
	CollisionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); 
	ExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	PulseExplosionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::GetPlayer, 2.0f);
    
	// Create the grenade indicator if not already created 
	if(!Indicator)
	{
		Indicator = GetWorld()->SpawnActor<AActor>(IndicatorActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
		Indicator->SetActorHiddenInGame(true); // Not shown as default 
	}

	DisableGrenade();
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

	DOREPLIFETIME(ALightGrenade,  bIsExploding);
}


void ALightGrenade::Throw(const float TimeHeld)
{
	if(bCanThrow)
	{
		Damage = 15.0f;
		PulseExplosionArea->Deactivate();
		ServerRPCThrow(TimeHeld);

		Indicator->SetActorHiddenInGame(true);
		MaxThrowIterations = 0;
		bIncreasingCharge = true; 
		GetWorld()->GetTimerManager().ClearTimer(ThrowIterTimerHandle); 
	}
}

void ALightGrenade::ServerRPCThrow_Implementation(const float TimeHeld)
{
	if(!this->HasAuthority())
		return;

	MulticastRPCThrow(TimeHeld);
}

void ALightGrenade::MulticastRPCThrow_Implementation(const float TimeHeld)
{
	if(bCanThrow)
	{
		if (!Player || !PlayerBase)
		{
			GetPlayer();
		}

		if (PlayerBase)
		{
			Controller = PlayerBase->GetInstigatorController();
		}
		
		
		UE_LOG(LogTemp, Warning, TEXT("Throw Grenade"));
	
		

		ThrowEvent();

		bIsExploding = false;
		

		ExplosionArea->SetWorldLocation(Player->ThrowLoc->GetComponentLocation()); 
		
		ExplosionArea->AddImpulse(GetLaunchForce(TimeHeld));

		EnableGrenade();
		
		bCanThrow = false;

		StartCountdown(ExplodeTimeSlow);
	}
}


void ALightGrenade::ActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	if (!HasAuthority()) // Only run on server
		return;
	
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor))
		{
			
			StartCountdown(ExplodeTimeFast);
			
		}else if (APressurePlateBase* Plate = Cast<APressurePlateBase>(OtherActor))
		{
			Plate->StartMove();		
		}/*
		else if (OtherActor || OtherComp || OverlappedComponent)
		{

			StartCountdown(ExplodeTimeSlow);
			
		} */
	
}

void ALightGrenade::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APressurePlateBase* Plate = Cast<APressurePlateBase>(OtherActor))
	{
		Plate->StartReverse();		
	}
}

void ALightGrenade::ServerRPCExplosion_Implementation()
{
	// Code here is only run on server, will probably not be changed unless we'll have server specific behaviour 
	
	// Should only run on server 
	if(!this->HasAuthority())
		return;

	if (!bIsExploding)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Explode"));
		bIsExploding = true;
		MulticastRPCExplosion();
	}
	
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
			UE_LOG(LogClass, Log, TEXT("did dmg"));
			
			OverlapingActor->TakeDamage(Damage, FDamageEvent(), Controller, this);
		}
		
		
	}
	
	
}

void ALightGrenade::IncreaseMaxThrowIterations()
{
	if(bCanThrow)
	{
		MaxThrowIterations++;
		bIncreasingCharge = !bIncreasingCharge; 
	}
}

FVector ALightGrenade::GetLaunchForce(const float TimeHeld)
{
	if (!Player)
	{
		GetPlayer();
	}
	
	// Get the direction to throw the grenade in, check if depth movement is enabled 
	FVector ThrowDir = Player->ThrowLoc->GetComponentLocation() - Player->GetActorLocation();
	if(!Player->IsDepthMovementEnabled())
		ThrowDir.X = 0; 

	FVector ThrowImpulse; 
	// Calculate the force and clamp it to ensure it is between set bounds and adjust to looping throw force
	if(bIncreasingCharge)
	{
		ThrowImpulse = ThrowDir.GetSafeNormal() *
			(StartThrowImpulse + TimeHeld * FireSpeedPerSecondHeld - MaxThrowImpulse * MaxThrowIterations);
	} else
	{
		ThrowImpulse = ThrowDir.GetSafeNormal() *
			(MaxThrowImpulse - (TimeHeld * FireSpeedPerSecondHeld - MaxThrowImpulse * MaxThrowIterations));
	}

	// Impulse greater than max force, call timer to increase throw loops after set delay if not already called 
	if(!GetWorldTimerManager().IsTimerActive(ThrowIterTimerHandle))
	{
		if((ThrowImpulse.Size() >= MaxThrowImpulse && bIncreasingCharge) || (ThrowImpulse.Size() <= StartThrowImpulse && !bIncreasingCharge))
			GetWorldTimerManager().SetTimer(ThrowIterTimerHandle, this, &ALightGrenade::IncreaseMaxThrowIterations, TimeAtMaxThrowForce);
	}
	
	return ThrowImpulse.GetClampedToSize(StartThrowImpulse, MaxThrowImpulse); 
}

void ALightGrenade::EnableCanThrow()
{
	bCanThrow = true;
}

void ALightGrenade::DisableGrenade()
{
	
	CollisionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GrenadeMesh->SetVisibility(false);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::EnableCanThrow, ThrowCooldown);
	
}

void ALightGrenade::EnableGrenade()
{
	CollisionArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GrenadeMesh->SetVisibility(true);
	
}

void ALightGrenade::GetPlayer()
{
	Player = Cast<ASoulCharacter>(Cast<ASonderGameState>(UGameplayStatics::GetGameState(this))->GetServerPlayer());
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player"));
		Player = Cast<ASoulCharacter>(Cast<ASonderGameState>(UGameplayStatics::GetGameState(this))->GetClientPlayer());
	}
	
	PlayerBase = Cast<APROJCharacter>(Player);
}

void ALightGrenade::StartCountdown(float TimeUntilExplosion)
{
	UE_LOG(LogTemp, Warning, TEXT("Countdown Started"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ALightGrenade::ServerRPCExplosion, TimeUntilExplosion);
}

void ALightGrenade::PulseExplosion()
{
	Damage = 20.0f;
	PulseExplosionArea->Activate();
}

void ALightGrenade::IsChargingGrenade(const float TimeHeld)
{
	if(!bCanThrow)
		return; 
	
	FHitResult HitResult;
	FVector OutLastTraceDestination;
	TArray<FVector> PathLocs;
	const TArray<AActor*> ActorsToIgnore { Player, this, Indicator }; 
	
	const bool bHit = UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel
	(this, HitResult,
		PathLocs, OutLastTraceDestination , Player->ThrowLoc->GetComponentLocation(), GetLaunchForce(TimeHeld) / IndicatorOffsetDivisor,
		true, CollisionArea->GetScaledSphereRadius() / 2, ECC_Pawn, false,
		ActorsToIgnore, EDrawDebugTrace::None, -1);

	if(!bHit)
		return; 

	// Set indicators' location to the impact point 
	Indicator->SetActorHiddenInGame(false); 
	Indicator->SetActorLocation(HitResult.ImpactPoint);
}

void ALightGrenade::DisableIndicator() const
{
	if(Indicator)
		Indicator->SetActorHiddenInGame(true); 
}

