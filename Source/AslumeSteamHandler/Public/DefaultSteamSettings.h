// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DefaultSteamSettings.generated.h"

/**
 * 
 */
UCLASS(Config = DefaultSteamSettings, defaultconfig, meta = (DisplayName = "Steam Settings"), Category = "SteamSettings")
class ASLUMESTEAMHANDLER_API UDefaultSteamSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UDefaultSteamSettings();

	UPROPERTY(Config, EditAnywhere, meta = (ClampMin = "0", UIMin = "0"), Category = "SteamSettings")
	int AppID;

	UPROPERTY(Config, EditAnywhere, Category = "SteamSettings")
	FString SomeText;
};
