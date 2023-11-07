// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkManager.h"

#include "SocketSubsystem.h"

// Sets default values
ANetworkManager::ANetworkManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANetworkManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANetworkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


FString ANetworkManager::GetIpClean()
{
	bool bCanBindAll;
	TSharedPtr<class FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(
		*GLog, bCanBindAll);
	FString MyIP = Addr->ToString(false);

	return MyIP;
}
