// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#include "AslumeSteamHandler.h"

#include "Interfaces/IPluginManager.h"

#include "Steam/steam_api.h"

#define LOCTEXT_NAMESPACE "FAslumeSteamHandlerModule"

DEFINE_LOG_CATEGORY_STATIC(AslumeSteamHandler, Log, All);

void FAslumeSteamHandlerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("AslumeSteamHandler")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;

#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SteamHandlerLibrary/RedistributableBin/win64/steam_api64.dll"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SteamHandlerLibrary/RedistributableBin/osx/libsteam_api.dylib"));
#endif // PLATFORM_WINDOWS


	SteamLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (SteamLibraryHandle ==  nullptr)
	{
		// Error and termination
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("AslumeSteamHandler", "Failed to load Steam Handler Library"));
		UE_LOG(AslumeSteamHandler, Error, TEXT("Failed to load Steam Handler Library"));
		return;
	}

	SteamAPI_Init();
}

void FAslumeSteamHandlerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(SteamLibraryHandle);
	SteamLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAslumeSteamHandlerModule, AslumeSteamHandler)