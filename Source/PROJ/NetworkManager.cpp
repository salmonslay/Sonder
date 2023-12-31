// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkManager.h"

#include "SocketSubsystem.h"

// Sets default values
ANetworkManager::ANetworkManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

FString ANetworkManager::GetLocalAddress()
{
	bool bCanBindAll;
	const TSharedPtr<class FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(
		*GLog, bCanBindAll);
	FString MyIP = Addr->ToString(false);

	return MyIP;
}

FString ANetworkManager::LocalAddressToShortcode(FString IPv4)
{
	IPv4 = IPv4.ToUpper(); // case-insensitive because there's no reason not to :)
	
	for (auto& Elem : ShortcodeToIPv4Map)
	{
		if (IPv4.StartsWith(Elem.Value))
			return IPv4.Replace(*Elem.Value, *Elem.Key); // return a shortcode if the three first octets match
	}

	return IPv4;
}

FString ANetworkManager::ShortcodeToLocalAddress(FString Shortcode)
{
	Shortcode = Shortcode.ToUpper();
	
	for (auto& Elem : ShortcodeToIPv4Map)
	{
		if (Shortcode.StartsWith(Elem.Key))
			return Shortcode.Replace(*Elem.Key, *Elem.Value);
	}

	return Shortcode;
}
