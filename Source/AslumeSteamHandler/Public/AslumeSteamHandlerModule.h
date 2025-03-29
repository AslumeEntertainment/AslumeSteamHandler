// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FOnlineFactoryASH;

DECLARE_LOG_CATEGORY_EXTERN(AslumeSteamHandler, Log, All);

class FAslumeSteamHandlerModule : public IModuleInterface
{


private:
	/** Handle to the test dll we will load */
	void* SteamLibraryHandle = nullptr;

	FOnlineFactoryASH* ASHFactory = nullptr;

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	
};
