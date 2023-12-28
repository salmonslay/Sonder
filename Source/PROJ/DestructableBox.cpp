// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructableBox.h"

#include "BaseHealthComponent.h"
#include "PROJCharacter.h"
#include "Components/BoxComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADestructableBox::ADestructableBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UBaseHealthComponent>(TEXT("HealthComponent"));

	DestructibleBox = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("DestructibleBox"));
	SetRootComponent(DestructibleBox);
	
	CollBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollBox"));
	CollBox->SetupAttachment(RootComponent);
	
	DestructibleBox->SetSimulatePhysics(false);
}

void ADestructableBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ADestructableBox, HealthComponent);
}

float ADestructableBox::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = HealthComponent->TakeDamage(DamageApplied);

	if (HealthComponent->IsDead())
		Crumble();

	return DamageApplied;
}

void ADestructableBox::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bDestroyed)
		Shrink(DeltaSeconds); 
}

void ADestructableBox::Crumble()
{
	if(bDestroyed)
		return;

	ServerRPC_Crumble(); 
}

void ADestructableBox::ServerRPC_Crumble_Implementation()
{
	if(!HasAuthority())
		return;

	MulticastRPC_Crumble(); 
}

void ADestructableBox::MulticastRPC_Crumble_Implementation()
{
	DestructibleBox->SetSimulatePhysics(true);
	CollBox->DestroyComponent(); 
	
	// disable collision with players (NOTE: this does not disable collisions with the broken up pieces)
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(this, APROJCharacter::StaticClass(), Players);

	for(const auto Player : Players)
	{
		DestructibleBox->IgnoreActorWhenMoving(Player, true);
		Cast<APawn>(Player)->MoveIgnoreActorAdd(this);
	}

	// triggers destruction 
	DestructibleBox->CrumbleActiveClusters();

	bDestroyed = true;

	OnDeath();
}

void ADestructableBox::Shrink(const float DeltaTime)
{
	// decrease scale 
	SetActorScale3D(UKismetMathLibrary::VInterpTo_Constant(GetActorScale3D(),FVector::Zero(), DeltaTime, ShrinkSpeed));
	
	// destroy when scale reaches zero  
	if(GetActorScale3D().IsNearlyZero())
		Destroy();
}

