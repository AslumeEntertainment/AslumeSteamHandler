// Copyright (c) 2025, Aslume Entertainment. All rights reserved.


#include "Subsystems/ASH_DefaultSubsystem.h"

#include "Interfaces/IPluginManager.h"

#include "Steam/steam_api.h"

#define LOCTEXT_NAMESPACE "AslumeSteamHandler_DefaultSubsystem"

DEFINE_LOG_CATEGORY_STATIC(AslumeSteamHandler_DefaultSubsystem, Log, All);

bool UASH_DefaultSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UASH_DefaultSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	/*
	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("AslumeSteamHandler")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;

#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/SteamHandlerLibrary/RedistributableBin/win64/steam_api64.dll"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SteamHandlerLibrary/RedistributableBin/osx/libsteam_api.dylib"));
#endif // PLATFORM_WINDOWS

	SteamLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (SteamLibraryHandle == nullptr)
	{
		// Error and termination
		UE_LOG(AslumeSteamHandler_DefaultSubsystem, Error, TEXT("Failed to load Steam Handler Library"));
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("AslumeSteamHandler_DefaultSubsystem", "Failed to load Steam Handler Library"));
		return;
	}

	UE_LOG(AslumeSteamHandler_DefaultSubsystem, Display, TEXT("Steam Handler Library Loaded Sucessfully"));

	FFileHelper::SaveStringToFile(TEXT("480"), TEXT("steam_appid.txt"));

	SteamAPI_RestartAppIfNecessary(480);

	if (SteamAPI_Init() == false)
	{
		UE_LOG(AslumeSteamHandler_DefaultSubsystem, Error, TEXT("Failed to Initialize Steam"));
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("AslumeSteamHandler_DefaultSubsystem", "Failed to Initialize Steam"));
		return;
	}

	UE_LOG(AslumeSteamHandler_DefaultSubsystem, Display, TEXT("Steam Initialized Sucessfully"));*/
}

void UASH_DefaultSubsystem::Deinitialize()
{
	// Free the dll handle
	FPlatformProcess::FreeDllHandle(SteamLibraryHandle);
	SteamLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
