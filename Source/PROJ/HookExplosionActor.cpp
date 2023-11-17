// Fill out your copyright notice in the Description page of Project Settings.


#include "HookExplosionActor.h"

#include "EnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AHookExplosionActor::AHookExplosionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(FName("Overlap Sphere"));
}

// Run before begin play, serving as the class' "constructor" 
void AHookExplosionActor::Initialize(const float HookTravelDistance, APawn* RobotCharacter)
{
	HookShotTravelDistance = HookTravelDistance;

	RobotChar = RobotCharacter; 
}

// Called when the game starts or when spawned
void AHookExplosionActor::BeginPlay()
{
	Super::BeginPlay();

	DamageOverlap();

	if(ExplosionNS)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionNS, GetActorLocation()); 
}

void AHookExplosionActor::DamageOverlap()
{
	// This function is used instead of checking what overlaps with the OverlapSphere so overlapping actors on creation
	// are registered 
	TArray<AActor*> OverlappingActors;
	UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), OverlapSphere->GetScaledSphereRadius(), OverlapCollisionObjects, AEnemyCharacter::StaticClass(), TArray<AActor*>(), OverlappingActors); 

	const float DamageAmount = GetDamageAmount();
	// UE_LOG(LogTemp, Warning, TEXT("Travel dist: %f - Explosion Damage: %f"), HookShotTravelDistance, DamageAmount)

	for(const auto Actor : OverlappingActors)
		Actor->TakeDamage(DamageAmount, FDamageEvent(), RobotChar->GetInstigatorController(), this); 
}

float AHookExplosionActor::GetDamageAmount() const
{
	return FMath::Max((ExplosionDamageAmountPerMeterTraveled / 100) * HookShotTravelDistance, MinimumDamageAmount); 
}
