// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ASH_DefaultSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ASLUMESTEAMHANDLER_API UASH_DefaultSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	/** Handle to the test dll we will load */
	void* SteamLibraryHandle = nullptr;

protected:
	/**
	* Determines whether the subsystem should be creted. Natively turns of the creation unless the class is blueprint successor
	*
	* @param Outer The onwning object of the subsystem
	*/
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/**
	* Setting up the default inbound delegates
	*
	* @param Collection - The onwning object of the subsystem
	*/
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	Cleaning up the default inbound delegates
	*
	*/
	virtual void Deinitialize() override;
};
