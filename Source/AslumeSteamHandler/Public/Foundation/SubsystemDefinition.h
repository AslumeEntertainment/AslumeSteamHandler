// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once 

#include "OnlineSubsystemNull.h"
#include "OnlineSubsystem.h"

#ifndef ASH_SUBSYSTEM
const FName ASH_SUBSYSTEM(TEXT("ASH"));
#endif

DECLARE_LOG_CATEGORY_EXTERN(OnlineSubsystemASH, Log, All);

//typedef TSharedPtr<class FOnlineIdentitySteam, ESPMode::ThreadSafe> FOnlineIdentitySteamPtr;

typedef TSharedPtr<class FOnlineSessionASH, ESPMode::ThreadSafe> FOnlineSessionASHPtr;


class FOnlineSubsystemASH : public FOnlineSubsystemImpl
{
private:
	FOnlineSessionASHPtr SessionInterface = nullptr;

public:

	FOnlineSubsystemASH(FName InInstanceName) : FOnlineSubsystemImpl(ASH_SUBSYSTEM, InInstanceName)
	{

	}

	virtual ~FOnlineSubsystemASH() = default;

	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlinePartyPtr GetPartyInterface() const override;
	virtual IOnlineGroupsPtr GetGroupsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;
	virtual IOnlineTimePtr GetTimeInterface() const override;
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
	virtual IOnlinePurchasePtr GetPurchaseInterface() const override;
	virtual IOnlineEventsPtr GetEventsInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineSharingPtr GetSharingInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlineMessagePtr GetMessageInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual IOnlineChatPtr GetChatInterface() const override;
	virtual IOnlineStatsPtr GetStatsInterface() const override;
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override;
	virtual IOnlineTournamentPtr GetTournamentInterface() const override;
	virtual IMessageSanitizerPtr GetMessageSanitizer(int32 LocalUserNum, FString& OutAuthTypeToExclude) const override;

	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual FText GetOnlineServiceName() const override;

};


typedef TSharedPtr<FOnlineSubsystemASH, ESPMode::ThreadSafe> FOnlineSubsystemASHPtr;


class FOnlineFactoryASH : public IOnlineFactory
{
public:

	FOnlineFactoryASH() {}
	virtual ~FOnlineFactoryASH() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override;
};