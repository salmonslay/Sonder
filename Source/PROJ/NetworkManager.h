// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetworkManager.generated.h"

UCLASS()
class PROJ_API ANetworkManager : public AActor // i'm not sure why this is an actor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANetworkManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * @return The local IPv4 address of this computer 
	 */
	UFUNCTION(BlueprintPure)
	static FString GetLocalAddress();

	/**
	 * Tries to convert an IPv4 address to a shortcode. If the address is not in the map, it will return the original address.
	 * @param IPv4 The address to convert
	 * @return A shortcode if possible, else the original address
	 * @note 192.168.0.34 will return X34.
	 */
	UFUNCTION(BlueprintPure)
	static FString LocalAddressToShortcode(FString IPv4);

	/**
	 * Tries to convert a shortcode to an IPv4 address. If the shortcode is not in the map, it will return the provided input.
	 * @param Shortcode The shortcode to convert
	 * @return An IPv4 address if possible, else the original input.
	 */
	UFUNCTION(BlueprintPure)
	static FString ShortcodeToLocalAddress(FString Shortcode);

	// Map of shortcodes to the first three octets of the IPv4 address.
	inline static TMap<FString, FString> ShortcodeToIPv4Map = {
		{
			"G", "193.10.9." // the game lab
		},
		{
			"L", "192.168.1.", // most common
		},
		{
			"X", "192.168.0.", // also common
		}
	};
};
