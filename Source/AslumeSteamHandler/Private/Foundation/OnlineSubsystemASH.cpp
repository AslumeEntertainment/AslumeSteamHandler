// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#include "Foundation/OnlineSubsystemASH.h"

#include "OnlineSubsystemImpl.h"
#include "Interfaces/IPluginManager.h"
#include "SocketSubsystem.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

#include "Steam/steam_api.h"
#include "Steam/steam_gameserver.h"
//#include <steam/isteamgameserver.h>
//#include <steam/isteamgameserverstats.h>

#include "Interfaces/OnlineSessionASH.h"

DEFINE_LOG_CATEGORY(OnlineSubsystemASH);


namespace FNetworkProtocolTypes
{
	const FLazyName Steam(TEXT("Steam"));
}

namespace OSSConsoleVariables
{
	TAutoConsoleVariable<int32> CVarSteamInitServerOnClient(
		TEXT("OSS.SteamInitServerOnClient"),
		0,
		TEXT("Whether or not to initialize the Steam server interface on clients (default false)"),
		ECVF_Default | ECVF_Cheat);

#if !UE_BUILD_SHIPPING
	/** CVar used by NetcodeUnitTest, to force-enable Steam within the unit test commandlet */
	TAutoConsoleVariable<int32> CVarSteamUnitTest(
		TEXT("OSS.SteamUnitTest"),
		0,
		TEXT("Whether or not Steam is being force-enabled by NetcodeUnitTest"),
		ECVF_Default);
#endif
}


extern "C"
{
	static void __cdecl SteamworksWarningMessageHook(int Severity, const char* Message);
	static void __cdecl SteamworksWarningMessageHookNoOp(int Severity, const char* Message);
}

/**
 * Callback function into Steam error messaging system
 * @param Severity - error level
 * @param Message - message from Steam
 */
static void __cdecl SteamworksWarningMessageHook(int Severity, const char* Message)
{
	const TCHAR* MessageType;
	switch (Severity)
	{
	case 0: MessageType = TEXT("message"); break;
	case 1: MessageType = TEXT("warning"); break;
	default: MessageType = TEXT("notification"); break;  // Unknown severity; new SDK?
	}
	UE_LOG(OnlineSubsystemASH, Warning, TEXT("Steamworks SDK %s: %s"), MessageType, UTF8_TO_TCHAR(Message));
}

/**
 * Callback function into Steam error messaging system that outputs nothing
 * @param Severity - error level
 * @param Message - message from Steam
 */
static void __cdecl SteamworksWarningMessageHookNoOp(int Severity, const char* Message)
{
	// no-op.
}


//---------------------------------------------Factory--------------------------------------------

IOnlineSubsystemPtr FOnlineFactoryASH::CreateSubsystem(FName InstanceName)
{
	FOnlineSubsystemASHPtr OnlineSub = MakeShareable(new FOnlineSubsystemASH(InstanceName));

	if (!OnlineSub->Init())
	{
		UE_LOG(OnlineSubsystemASH, Error, TEXT("CreateSubsystem - Aslume Steam Handler Online Subsystem failed to initialize!"));
		OnlineSub->Shutdown();
		OnlineSub = NULL;
		return nullptr;
	}

	UE_LOG(OnlineSubsystemASH, Display, TEXT("CreateSubsystem - Aslume Steam Handler Online Subsystem initialized properly!"));
	return OnlineSub;
}
//--------------------------------------------Factory--------------------------------------------



//---------------------------------------File Work----------------------------------------

inline FString GetSteamAppIdFilename()
{
	return FString::Printf(TEXT("%s%s"), FPlatformProcess::BaseDir(), STEAMAPPIDFILENAME);
}

#if !UE_BUILD_SHIPPING && !UE_BUILD_SHIPPING_WITH_EDITOR
/**
 * Write out the steam app id to the steam_appid.txt file before initializing the API
 * @param SteamAppId id assigned to the application by Steam
 */
static bool WriteSteamAppIdToDisk(int32 SteamAppId)
{
	if (SteamAppId > 0)
	{
		// Access the physical file writer directly so that we still write next to the executable in CotF builds.
		FString SteamAppIdFilename = GetSteamAppIdFilename();
		TUniquePtr<IFileHandle> Handle(IPlatformFile::GetPlatformPhysical().OpenWrite(*SteamAppIdFilename, false, false));
		if (!Handle)
		{
			UE_LOG(OnlineSubsystemASH, Error, TEXT("Failed to create file: %s"), *SteamAppIdFilename);
			return false;
		}
		else
		{
			TAnsiStringBuilder<16> AppId(InPlace, SteamAppId);
			Handle->Write((const uint8*)AppId.GetData(), AppId.Len());
			return true;
		}
	}

	UE_LOG(OnlineSubsystemASH, Warning, TEXT("Steam App Id provided (%d) is invalid, must be greater than 0!"), SteamAppId);
	return false;
}

/**
 * Deletes the app id file from disk
 */
static void DeleteSteamAppIdFromDisk()
{
	const FString SteamAppIdFilename = GetSteamAppIdFilename();
	IPlatformFile& Physical = IPlatformFile::GetPlatformPhysical();
	if (Physical.FileExists(*SteamAppIdFilename))
	{
		Physical.DeleteFile(*SteamAppIdFilename);
	}
}

#endif // !UE_BUILD_SHIPPING && !UE_BUILD_SHIPPING_WITH_EDITOR

bool ConfigureSteamInitDevOptions(bool& RequireRelaunch, int32& RelaunchAppId)
{
#if !UE_BUILD_SHIPPING && !UE_BUILD_SHIPPING_WITH_EDITOR
	// Write out the steam_appid.txt file before launching
	if (!GConfig->GetInt(TEXT("OnlineSubsystemASH"), TEXT("SteamAppId"), RelaunchAppId, GEngineIni))
	{
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("Missing SteamAppId key in OnlineSubsystemASH of DefaultEngine.ini"));
		RelaunchAppId = -1;
		return false;
	}
	else
	{
		if (!WriteSteamAppIdToDisk(RelaunchAppId))
		{
			UE_LOG(OnlineSubsystemASH, Warning, TEXT("Could not create/update the steam_appid.txt file! Make sure the directory is writable and there isn't another instance using this file"));
			return false;
		}
	}

	// Should the game force a relaunch in Steam if the client isn't already loaded
	if (!GConfig->GetBool(TEXT("OnlineSubsystemASH"), TEXT("bRelaunchInSteam"), RequireRelaunch, GEngineIni))
	{
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("ConfigureSteamInitDevOptions - Missing bRelaunchInSteam key in OnlineSubsystemASH of DefaultEngine.ini falling back to default 'false'"));
	}

#else
	// Always check against the Steam client when shipping
	RequireRelaunch = true;
	if (!GConfig->GetInt(TEXT("OnlineSubsystemASH"), TEXT("SteamAppId"), RelaunchAppId, GEngineIni))
	{
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("Missing SteamAppId key in OnlineSubsystemASH of DefaultEngine.ini"));
		RelaunchAppId = -1;
		return false;
	}
#endif

	return true;
}
//---------------------------------------File Work----------------------------------------


//---------------------------------------Initialization----------------------------------------

bool FOnlineSubsystemASH::InitSteamworksClient(bool bRelaunchInSteam, int32 SteamAppId)
{
	bSteamworksClientInitialized = false;

	// If the game was not launched from within Steam, but is supposed to, trigger a Steam launch and exit
	if (bRelaunchInSteam && SteamAppId != 0 && SteamAPI_RestartAppIfNecessary(SteamAppId))
	{
		if (FPlatformProperties::IsGameOnly() || FPlatformProperties::IsServerOnly())
		{
			UE_LOG(OnlineSubsystemASH, Log, TEXT("Game restarting within Steam client, exiting"));
			FPlatformMisc::RequestExit(false);
		}

		return bSteamworksClientInitialized;
	}
	// Otherwise initialize as normal
	else
	{
		//SteamAPIClientHandle = FSteamSharedModule::Get().ObtainSteamClientInstanceHandle();
		// Steamworks needs to initialize as close to start as possible, so it can hook its overlay into Direct3D, etc.
		bSteamworksClientInitialized = SteamAPI_Init();// (SteamAPIClientHandle.IsValid() ? true : false);

		// Test all the Steam interfaces
#define GET_STEAMWORKS_INTERFACE(Interface) \
		if (Interface() == nullptr) \
		{ \
			UE_LOG(OnlineSubsystemASH, Warning, TEXT("InitSteamworksClient - Steamworks: %s() failed!"), TEXT(#Interface)); \
			bSteamworksClientInitialized = false; \
		} \

		// GSteamUtils
		GET_STEAMWORKS_INTERFACE(SteamUtils);
		// GSteamUser
		GET_STEAMWORKS_INTERFACE(SteamUser);
		// GSteamFriends
		GET_STEAMWORKS_INTERFACE(SteamFriends);
		// GSteamRemoteStorage
		GET_STEAMWORKS_INTERFACE(SteamRemoteStorage);
		// GSteamUserStats
		GET_STEAMWORKS_INTERFACE(SteamUserStats);
		// GSteamMatchmakingServers
		GET_STEAMWORKS_INTERFACE(SteamMatchmakingServers);
		// GSteamApps
		GET_STEAMWORKS_INTERFACE(SteamApps);
		// GSteamNetworking
		GET_STEAMWORKS_INTERFACE(SteamNetworking);
		// GSteamMatchmaking
		GET_STEAMWORKS_INTERFACE(SteamMatchmaking);

#undef GET_STEAMWORKS_INTERFACE
	}

	if (bSteamworksClientInitialized)
	{
		if (FParse::Value(FCommandLine::Get(), TEXT("GamePort="), GameServerGamePort) == false)
		{
			if (!GConfig->GetInt(TEXT("OnlineSubSystemASH"), TEXT("GamePort"), GameServerGamePort, GEngineIni))
			{
				UE_LOG(OnlineSubsystemASH, Warning, TEXT("InitSteamworksClient - Missing GamePort key in OnlineSubsystemASH of DefaultEngine.ini and Commandline vars, falling back to default '7777'"));
				GameServerGamePort = 7777;// SteamAPIClientHandle->GetGamePort();
			}
		}
		bool bIsSubscribed = true;
		if (FPlatformProperties::IsGameOnly() || FPlatformProperties::IsServerOnly())
		{
			bIsSubscribed = SteamApps()->BIsSubscribed();
		}

		// Make sure the Steam user has valid access to the game
		if (bIsSubscribed)
		{
			UE_LOG(OnlineSubsystemASH, Log, TEXT("InitSteamworksClient - Steam User is subscribed %i"), bSteamworksClientInitialized);
			if (SteamUtils())
			{
				SteamAppID = SteamUtils()->GetAppID();
				SteamUtils()->SetWarningMessageHook(SteamworksWarningMessageHook);
			}
		}
		else
		{
			UE_LOG(OnlineSubsystemASH, Error, TEXT("InitSteamworksClient - Steam User is NOT subscribed, exiting."));
			bSteamworksClientInitialized = false;
			FPlatformMisc::RequestExit(false);
		}
	}

	UE_LOG(OnlineSubsystemASH, Log, TEXT("InitSteamworksClient - [AppId: %d] Client API initialized %i"), GetSteamAppId(), bSteamworksClientInitialized);
	return bSteamworksClientInitialized;
}

bool FOnlineSubsystemASH::InitSteamworksServer()
{
	//SteamAPIServerHandle = FSteamSharedModule::Get().ObtainSteamServerInstanceHandle();
	bSteamworksGameServerInitialized = true;// (SteamAPIServerHandle.IsValid());

	if (bSteamworksGameServerInitialized)
	{
		// Grab the port values so that we save them.
		GameServerGamePort = 0;// SteamAPIServerHandle->GetGamePort();
		GameServerQueryPort = 0;// SteamAPIServerHandle->GetQueryPort();

		// Test all the Steam interfaces
#define GET_STEAMWORKS_INTERFACE(Interface) \
		if (Interface() == nullptr) \
		{ \
			UE_LOG(OnlineSubsystemASH, Warning, TEXT("InitSteamworksServer - Steamworks: %s() failed!"), TEXT(#Interface)); \
			bSteamworksGameServerInitialized = false; \
		} \

		// NOTE: It's not possible for >some< of the interfaces to initialize, and others fail; it's all or none
		GET_STEAMWORKS_INTERFACE(SteamGameServer);
		GET_STEAMWORKS_INTERFACE(SteamGameServerStats);
		GET_STEAMWORKS_INTERFACE(SteamGameServerNetworking);
		GET_STEAMWORKS_INTERFACE(SteamGameServerUtils);

#undef GET_STEAMWORKS_INTERFACE
	}

	if (SteamGameServerUtils() != nullptr)
	{
		SteamAppID = SteamGameServerUtils()->GetAppID();
		SteamGameServerUtils()->SetWarningMessageHook(SteamworksWarningMessageHook);
	}

	// Get the multihome address. If there isn't one, this server will be set to listen on the any address.
	uint32 LocalServerIP = 0;
	FString MultiHome;
	if (FParse::Value(FCommandLine::Get(), TEXT("MULTIHOME="), MultiHome) && !MultiHome.IsEmpty())
	{
		TSharedPtr<FInternetAddr> MultiHomeIP = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetAddressFromString(MultiHome);
		if (MultiHomeIP.IsValid())
		{
			MultiHomeIP->GetIp(LocalServerIP);
		}
	}

	// Allow the command line to override the default query port for server list queries
	if (FParse::Value(FCommandLine::Get(), TEXT("QueryPort="), GameServerQueryPort) == false)
	{
		if (!GConfig->GetInt(TEXT("OnlineSubsystemASH"), TEXT("GameServerQueryPort"), GameServerQueryPort, GEngineIni))
		{
			UE_LOG(OnlineSubsystemASH, Warning, TEXT("InitSteamworksServer - Missing QueryPort key in OnlineSubsystemASH of DefaultEngine.ini and Commandline vars, falling back to default '27015'"));
			GameServerQueryPort = 27015;
		}
	}

	// Set VAC
	bool bVACEnabled = false;
	if (GConfig->GetBool(TEXT("OnlineSubsystemASH"), TEXT("bVACEnabled"), bVACEnabled, GEngineIni))
	{
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("InitSteamworksServer - Missing bVACEnabled key in OnlineSubsystemASH of DefaultEngine.ini, falling back to default 'false'"));
	}

	// Set GameVersions
	FString GameVersion;
	GConfig->GetString(TEXT("OnlineSubsystemASH"), TEXT("GameVersion"), GameVersion, GEngineIni);
	if (GameVersion.Len() == 0)
	{
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("InitSteamworksServer - Missing GameVersion key in OnlineSubsystemASH of DefaultEngine.ini, falling back to default ''"));
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("InitSteamworksServer - [OnlineSubsystemASH].GameVersion is not set. Server advertising will fail"));
	}

	UE_LOG(OnlineSubsystemASH, Display, TEXT("InitSteamworksServer - Initializing Steam Game Server IP: 0x%08X GamePort: %d QueryPort: %d"), LocalServerIP, GameServerGamePort, GameServerQueryPort);

	bSteamworksGameServerInitialized = SteamGameServer_Init(LocalServerIP, GameServerGamePort, GameServerQueryPort,
		(bVACEnabled ? eServerModeAuthenticationAndSecure : eServerModeAuthentication),
		TCHAR_TO_UTF8(*GameVersion));

	if(bSteamworksGameServerInitialized)
	{
		UE_LOG(OnlineSubsystemASH, Log, TEXT("InitSteamworksServer - [AppId: %d] Game Server API initialized %i"), GetSteamAppId(), bSteamworksGameServerInitialized);
	}
	else
	{
		UE_LOG(OnlineSubsystemASH, Error, TEXT("InitSteamworksServer - Dedicated Server API failed to initialize."));
	}
	
	return bSteamworksGameServerInitialized;
	
}

bool FOnlineSubsystemASH::Init()
{
	UE_LOG(LogTemp, Error, TEXT("-----------------------------------------------INITING THE ASHOSS-------------------------"));
	
	bool bRelaunchInSteam = false;
	int RelaunchAppId = 0;

	if (!ConfigureSteamInitDevOptions(bRelaunchInSteam, RelaunchAppId))
	{
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("Init - Could not set up the steam environment! Falling back to another OSS."));
		return false;
	}

	const bool bIsServer = IsRunningDedicatedServer();
	bool bInitServerOnClient = false;
	if (!GConfig->GetBool(TEXT("OnlineSubsystemASH"), TEXT("bInitServerOnClient"), bInitServerOnClient, GEngineIni))
	{
		UE_LOG(OnlineSubsystemASH, Warning, TEXT("Init - Missing bInitServerOnClient key in OnlineSubsystemASH of DefaultEngine.ini falling back to default 'false'"));
	}
	bool bAttemptServerInit = bIsServer || !!OSSConsoleVariables::CVarSteamInitServerOnClient.GetValueOnGameThread() || bInitServerOnClient;

	UE_LOG(OnlineSubsystemASH, Display, TEXT("Init - Steam: Starting SteamWorks. Client [%d] Server [%d]"), !bIsServer, bAttemptServerInit);

	// Don't initialize the Steam Client API if we are launching as a server
	bool bClientInitSuccess = !bIsServer ? InitSteamworksClient(bRelaunchInSteam, RelaunchAppId) : true;

	// Initialize the Steam Server API if this is a dedicated server or servers should initialize on clients
	bool bServerInitSuccess = bAttemptServerInit ? (InitSteamworksServer()) : true;

	if (bClientInitSuccess && bServerInitSuccess)
	{
		TSharedPtr<IPlugin> SocketsPlugin = IPluginManager::Get().FindPlugin(TEXT("SteamSockets"));
		if (!SocketsPlugin.IsValid() || (SocketsPlugin.IsValid() && !SocketsPlugin->IsEnabled()))
		{
			UE_LOG(OnlineSubsystemASH, Log, TEXT("Init - Initializing SteamNetworking Layer"));
			//CreateSteamSocketSubsystem();
			bUsingSteamNetworking = true;
		}

		// Create the online async task thread
		//OnlineAsyncTaskThreadRunnable = new FOnlineAsyncTaskManagerSteam(this);
		//check(OnlineAsyncTaskThreadRunnable);
		//OnlineAsyncTaskThread = FRunnableThread::Create(OnlineAsyncTaskThreadRunnable, *FString::Printf(TEXT("OnlineAsyncTaskThreadSteam %s"), *InstanceName.ToString()), 128 * 1024, TPri_Normal);
		//check(OnlineAsyncTaskThread);
		//UE_LOG(OnlineSubsystemASH, Display, TEXT("Created thread (ID:%d)."), OnlineAsyncTaskThread->GetThreadID());

		SessionInterface = MakeShareable(new FOnlineSessionASH(this));
		//SessionInterface->CheckPendingSessionInvite();

		//IdentityInterface = MakeShareable(new FOnlineIdentitySteam(this));

		//PresenceInterface = MakeShareable(new FOnlinePresenceSteam(this));

		//AuthInterfaceUtils = MakeShareable(new FOnlineAuthUtilsSteam());
		//AuthInterface = MakeShareable(new FOnlineAuthSteam(this, AuthInterfaceUtils));

		if (!bIsServer)
		{
			//FriendInterface = MakeShareable(new FOnlineFriendsSteam(this));
			//UserCloudInterface = MakeShareable(new FOnlineUserCloudSteam(this));
			//SharedCloudInterface = MakeShareable(new FOnlineSharedCloudSteam(this));
			//LeaderboardsInterface = MakeShareable(new FOnlineLeaderboardsSteam(this));
			//VoiceInterface = MakeShareable(new FOnlineVoiceSteam(this));
			//ExternalUIInterface = MakeShareable(new FOnlineExternalUISteam(this));
			//AchievementsInterface = MakeShareable(new FOnlineAchievementsSteam(this));
			//EncryptedAppTicketInterface = MakeShareable(new FOnlineEncryptedAppTicketSteam(this));
			//PurchaseInterface = MakeShareable(new FOnlinePurchaseSteam(this));
			//StoreInterface = MakeShareable(new FOnlineStoreSteam(this));

			// Kick off a download/cache of the current user's stats
			//LeaderboardsInterface->CacheCurrentUsersStats();
		}
		else
		{
			// Need a voice interface here to serialize packets but NOT add to VoiceData.RemotePackets
			//VoiceInterface = MakeShareable(new FOnlineVoiceSteam(this));
		}
	}
	else
	{
		// If the client succeeded, but the server didn't, this could be because there's a server and client running on the same machine - inform the user
		if (bClientInitSuccess)
		{
			UE_LOG(OnlineSubsystemASH, Warning, TEXT("Init - Failed to initialize Steam, this could be due to a Steam server and client running on the same machine. Try running with -NOSTEAM on the cmdline to disable."));
		}
		Shutdown();
	}

	return bClientInitSuccess && bServerInitSuccess;

}
//---------------------------------------Initialization----------------------------------------


bool FOnlineSubsystemASH::Shutdown()
{
	//if (IdentityInterfaceSteam.IsValid())
	//{
	//	ensure(IdentityInterfaceSteam.IsUnique());
	//	IdentityInterfaceSteam = nullptr;
	//}

	UE_LOG(OnlineSubsystemASH, Display, TEXT("OnlineSubsystemASH::Shutdown()"));

	/*if (OnlineAsyncTaskThread)
	{
		// Destroy the online async task thread
		delete OnlineAsyncTaskThread;
		OnlineAsyncTaskThread = nullptr;
	}*/

	/*if (OnlineAsyncTaskThreadRunnable)
	{
		delete OnlineAsyncTaskThreadRunnable;
		OnlineAsyncTaskThreadRunnable = nullptr;
	}*/

	/*if (VoiceInterface.IsValid() && bVoiceInterfaceInitialized) {
		VoiceInterface->Shutdown();
	}*/

#define DESTRUCT_INTERFACE(Interface) \
	if (Interface.IsValid()) \
	{ \
		ensure(Interface.IsUnique()); \
		Interface = nullptr; \
	}

	// Destruct the interfaces
	//DESTRUCT_INTERFACE(EncryptedAppTicketInterface);
	//DESTRUCT_INTERFACE(AchievementsInterface);
	//DESTRUCT_INTERFACE(ExternalUIInterface);
	//DESTRUCT_INTERFACE(VoiceInterface);
	//DESTRUCT_INTERFACE(LeaderboardsInterface);
	//DESTRUCT_INTERFACE(SharedCloudInterface);
	//DESTRUCT_INTERFACE(UserCloudInterface);
	//DESTRUCT_INTERFACE(FriendInterface);
	//DESTRUCT_INTERFACE(IdentityInterface);
	//DESTRUCT_INTERFACE(AuthInterface);
	//DESTRUCT_INTERFACE(AuthInterfaceUtils);
	//DESTRUCT_INTERFACE(PingInterface);
	DESTRUCT_INTERFACE(SessionInterface);
	//DESTRUCT_INTERFACE(PresenceInterface);
	//DESTRUCT_INTERFACE(StoreInterface);
	//DESTRUCT_INTERFACE(PurchaseInterface);

#undef DESTRUCT_INTERFACE

	//ClearUserCloudFiles();

	if (bUsingSteamNetworking)
	{
		//DestroySteamSocketSubsystem();
	}

	ShutdownSteamworks();

#if !UE_BUILD_SHIPPING && !UE_BUILD_SHIPPING_WITH_EDITOR
	DeleteSteamAppIdFromDisk();
#endif // !UE_BUILD_SHIPPING && !UE_BUILD_SHIPPING_WITH_EDITOR

	return FOnlineSubsystemImpl::Shutdown();
}

void FOnlineSubsystemASH::ShutdownSteamworks()
{
	UE_LOG(OnlineSubsystemASH, Display, TEXT("Unloading the Steam API..."));
	

	if (bSteamworksGameServerInitialized)
	{
		if (SessionInterface.IsValid())
		{
			//SessionInterface->GameServerSteamId = nullptr;
			//SessionInterface->bSteamworksGameServerConnected = false;
		}
		bSteamworksGameServerInitialized = false;
		SteamGameServer_Shutdown();
	}

	if (bSteamworksClientInitialized)
	{
		SteamAPI_Shutdown();
		bSteamworksClientInitialized = false;
	}
}

FString FOnlineSubsystemASH::GetAppId() const
{
	return FString::Printf(TEXT("%d"), GetSteamAppId());
}

bool FOnlineSubsystemASH::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar))
	{
		return true;
	}
	return false;
}

FText FOnlineSubsystemASH::GetOnlineServiceName() const
{
	return NSLOCTEXT("OnlineSubsystemASH", "OnlineServiceName", "Steam");
}

bool FOnlineSubsystemASH::Tick(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FOnlineSubsystemASH_Tick);

	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}
	if (SessionInterface.IsValid())
	{
		SessionInterface->Tick(DeltaTime);
	}

	return true;
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
