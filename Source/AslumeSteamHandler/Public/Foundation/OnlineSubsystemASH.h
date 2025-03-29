// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once 

#include "OnlineSubsystemImpl.h"

#include "CommonIncludes.h"

#define STEAMAPPIDFILENAME TEXT("steam_appid.txt")

DECLARE_LOG_CATEGORY_EXTERN(OnlineSubsystemASH, Log, All);

//typedef TSharedPtr<class FOnlineIdentitySteam, ESPMode::ThreadSafe> FOnlineIdentitySteamPtr;

typedef TSharedPtr<class FOnlineSessionASH, ESPMode::ThreadSafe> FOnlineSessionASHPtr;


class FOnlineSubsystemASH : public FOnlineSubsystemImpl
{
private:
	FOnlineSessionASHPtr SessionInterface = nullptr;

	bool bSteamworksClientInitialized;
	bool bSteamworksGameServerInitialized;

	bool bUsingSteamNetworking;

	uint32 SteamAppID;
	int32 GameServerGamePort;
	int32 GameServerQueryPort;

	bool InitSteamworksClient(bool bRelaunchInSteam, int32 SteamAppId);
	bool InitSteamworksServer();

	void ShutdownSteamworks();

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

	virtual bool Tick(float DeltaTime) override;

	/**
 * Whether or not the Steam Client interfaces are available; these interfaces are only available, if the Steam Client program is running
 * NOTE: These interfaces are made unavailable, when running a dedicated server
 *
 * @return	Whether or not the Steam Client interfaces are available
 */
	inline bool IsSteamClientAvailable()
	{
		return bSteamworksClientInitialized;
	}

	/**
	 * Whether or not the Steam game server interfaces are available; these interfaces are always available, so long as they were initialized correctly
	 * NOTE: The Steam Client does not need to be running for the game server interfaces to initialize
	 * NOTE: These interfaces are made unavailable, when not running a server
	 *
	 * @return	Whether or not the Steam game server interfaces are available
	 */
	inline bool IsSteamServerAvailable()
	{
		// @todo Steam - add some logic to detect somehow we intended to be a "Steam client" but failed that part
		// yet managed to still initialize the game server aspects of Steam
		return bSteamworksGameServerInitialized;
	}

	/**
	 * @return the steam app id for this app
	 */
	inline uint32 GetSteamAppId() const
	{
		return SteamAppID;
	}

	/**
	 *	@return the port the game has registered for play
	 */
	inline int32 GetGameServerGamePort() const
	{
		return GameServerGamePort;
	}

	/**
	 *	@return the port the game has registered for incoming server queries
	 */
	inline int32 GetGameServerQueryPort() const
	{
		return GameServerQueryPort;
	}

	/**
	 *	@return if this subsystem is using SteamNetworking functionality
	 *			or another network layer like SteamSockets
	 */
	inline bool IsUsingSteamNetworking() const
	{
		return bUsingSteamNetworking;
	}
};


typedef TSharedPtr<FOnlineSubsystemASH, ESPMode::ThreadSafe> FOnlineSubsystemASHPtr;


class FOnlineFactoryASH : public IOnlineFactory
{
public:

	FOnlineFactoryASH() {}
	virtual ~FOnlineFactoryASH() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override;
};