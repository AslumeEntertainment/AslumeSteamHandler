// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#include "AslumeSteamHandlerModule.h"

#define LOCTEXT_NAMESPACE "AslumeSteamHandlerModule"

#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h" //??
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#include "Foundation/OnlineSubsystemASH.h"

DEFINE_LOG_CATEGORY(AslumeSteamHandler);

void FAslumeSteamHandlerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	if (IsRunningGame() == false)
	{
		UE_LOG(AslumeSteamHandler, Warning, TEXT("Aslume Steam Handler will not load in editor. Only in standalone and packaged builds !"));
		return;
	}

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
		UE_LOG(AslumeSteamHandler, Error, TEXT("Failed to load Steam Handler Library"));
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("AslumeSteamHandler", "Failed to load Steam Handler Library"));
		return;
	}

	UE_LOG(AslumeSteamHandler, Display, TEXT("Steam Handler Library Loaded Sucessfully"));
	

	ASHFactory = new FOnlineFactoryASH();

	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(ASH_SUBSYSTEM, ASHFactory);

	/*FFileHelper::SaveStringToFile(TEXT("480"), TEXT("steam_appid.txt"));

	SteamAPI_RestartAppIfNecessary(480);

	if (!SteamAPI_Init())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Steam"));
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("AslumeSteamHandler", "Failed to Initialize Steam"));
		return;
	}

	UE_LOG(AslumeSteamHandler, Display, TEXT("Steam Initialized Sucessfully"));*/
}

void FAslumeSteamHandlerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (IsRunningGame() == false)
	{
		return;
	}
	
	//SteamAPI_Shutdown();

	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(ASH_SUBSYSTEM);

	delete ASHFactory;
	ASHFactory = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAslumeSteamHandlerModule, AslumeSteamHandler)