// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#include "Foundation/SubsystemDefinition.h"

#include "OnlineSubsystemImpl.h"

#include "Interfaces/OnlineSessionASH.h"

DEFINE_LOG_CATEGORY(OnlineSubsystemASH);

//---------------------------------------------Factory--------------------------------------------

IOnlineSubsystemPtr FOnlineFactoryASH::CreateSubsystem(FName InstanceName)
{
	FOnlineSubsystemASHPtr OnlineSub = MakeShareable(new FOnlineSubsystemASH(InstanceName));

	if (!OnlineSub->Init())
	{
		UE_LOG(OnlineSubsystemASH, Error, TEXT("Aslume Steam Handler Online Subsystem failed to initialize!"));
		OnlineSub->Shutdown();
		OnlineSub = NULL;
	}

	UE_LOG(OnlineSubsystemASH, Error, TEXT("Aslume Steam Handler Online Subsystem initialized properly!"));
	return OnlineSub;
}
//--------------------------------------------Factory--------------------------------------------



//---------------------------------------Online Subsystem----------------------------------------

bool FOnlineSubsystemASH::Init()
{
	UE_LOG(LogTemp, Error, TEXT("-----------------------------------------------INITING THE ASHOSS-------------------------"));
	SessionInterface = MakeShareable(new FOnlineSessionASH(this));
	return true;
	

}

bool FOnlineSubsystemASH::Shutdown()
{
	//if (IdentityInterfaceSteam.IsValid())
	//{
	//	ensure(IdentityInterfaceSteam.IsUnique());
	//	IdentityInterfaceSteam = nullptr;
	//}

	return FOnlineSubsystemImpl::Shutdown();
}

FString FOnlineSubsystemASH::GetAppId() const
{
	return FString();
}

bool FOnlineSubsystemASH::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

FText FOnlineSubsystemASH::GetOnlineServiceName() const
{
	return FText();
}

bool FOnlineSubsystemASH::Tick(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FOnlineSubsystemSteam_Tick);

	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("System Tick"));
	if (SessionInterface.IsValid())
	{
		SessionInterface->Tick(DeltaTime);
	}

	return false;
}
//---------------------------------------Online Subsystem----------------------------------------


//---------------------------------------Online Interfaces----------------------------------------

IOnlineSessionPtr FOnlineSubsystemASH::GetSessionInterface() const
{
	return SessionInterface;
}

IOnlineFriendsPtr FOnlineSubsystemASH::GetFriendsInterface() const
{
	return IOnlineFriendsPtr();
}

IOnlinePartyPtr FOnlineSubsystemASH::GetPartyInterface() const
{
	return IOnlinePartyPtr();
}

IOnlineGroupsPtr FOnlineSubsystemASH::GetGroupsInterface() const
{
	return IOnlineGroupsPtr();
}

IOnlineSharedCloudPtr FOnlineSubsystemASH::GetSharedCloudInterface() const
{
	return IOnlineSharedCloudPtr();
}

IOnlineUserCloudPtr FOnlineSubsystemASH::GetUserCloudInterface() const
{
	return IOnlineUserCloudPtr();
}

IOnlineEntitlementsPtr FOnlineSubsystemASH::GetEntitlementsInterface() const
{
	return IOnlineEntitlementsPtr();
}

IOnlineLeaderboardsPtr FOnlineSubsystemASH::GetLeaderboardsInterface() const
{
	return IOnlineLeaderboardsPtr();
}

IOnlineVoicePtr FOnlineSubsystemASH::GetVoiceInterface() const
{
	return IOnlineVoicePtr();
}

IOnlineExternalUIPtr FOnlineSubsystemASH::GetExternalUIInterface() const
{
	return IOnlineExternalUIPtr();
}

IOnlineTimePtr FOnlineSubsystemASH::GetTimeInterface() const
{
	return IOnlineTimePtr();
}

IOnlineIdentityPtr FOnlineSubsystemASH::GetIdentityInterface() const
{
	return IOnlineIdentityPtr();
}

IOnlineTitleFilePtr FOnlineSubsystemASH::GetTitleFileInterface() const
{
	return IOnlineTitleFilePtr();
}

IOnlineStoreV2Ptr FOnlineSubsystemASH::GetStoreV2Interface() const
{
	return IOnlineStoreV2Ptr();
}

IOnlinePurchasePtr FOnlineSubsystemASH::GetPurchaseInterface() const
{
	return IOnlinePurchasePtr();
}

IOnlineEventsPtr FOnlineSubsystemASH::GetEventsInterface() const
{
	return IOnlineEventsPtr();
}

IOnlineAchievementsPtr FOnlineSubsystemASH::GetAchievementsInterface() const
{
	return IOnlineAchievementsPtr();
}

IOnlineSharingPtr FOnlineSubsystemASH::GetSharingInterface() const
{
	return IOnlineSharingPtr();
}

IOnlineUserPtr FOnlineSubsystemASH::GetUserInterface() const
{
	return IOnlineUserPtr();
}

IOnlineMessagePtr FOnlineSubsystemASH::GetMessageInterface() const
{
	return IOnlineMessagePtr();
}

IOnlinePresencePtr FOnlineSubsystemASH::GetPresenceInterface() const
{
	return IOnlinePresencePtr();
}

IOnlineChatPtr FOnlineSubsystemASH::GetChatInterface() const
{
	return IOnlineChatPtr();
}

IOnlineStatsPtr FOnlineSubsystemASH::GetStatsInterface() const
{
	return IOnlineStatsPtr();
}

IOnlineTurnBasedPtr FOnlineSubsystemASH::GetTurnBasedInterface() const
{
	return IOnlineTurnBasedPtr();
}

IOnlineTournamentPtr FOnlineSubsystemASH::GetTournamentInterface() const
{
	return IOnlineTournamentPtr();
}

IMessageSanitizerPtr FOnlineSubsystemASH::GetMessageSanitizer(int32 LocalUserNum, FString& OutAuthTypeToExclude) const
{
	return IMessageSanitizerPtr();
}
