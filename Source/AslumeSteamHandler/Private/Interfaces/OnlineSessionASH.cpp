// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#include "Interfaces/OnlineSessionASH.h"

#include "Online/LANBeacon.h"
#include "Online/OnlineBase.h"
#include "OnlineSessionSettings.h"
#include "SocketSubsystem.h"

#include "Foundation/OnlineSubsystemASH.h"
#include "Foundation/OnlineSubsystemASHTypes.h"
#include "Misc/NboSerializerASH.h"

DEFINE_LOG_CATEGORY(OnlineSessionASH);


void CheckSettings(const FOnlineSessionSettings& Main, const FOnlineSessionSettings& Other)
{
	UE_LOG(LogTemp, Warning, TEXT("NumPublicConnections             - Main[%d] - Other[%d]"),Main.NumPublicConnections, Other.NumPublicConnections);
	UE_LOG(LogTemp, Warning, TEXT("NumPrivateConnections            - Main[%d] - Other[%d]"),Main.NumPrivateConnections, Other.NumPrivateConnections);
	UE_LOG(LogTemp, Warning, TEXT("bShouldAdvertise                 - Main[%d] - Other[%d]"),Main.bShouldAdvertise, Other.bShouldAdvertise);
	UE_LOG(LogTemp, Warning, TEXT("bAllowJoinInProgress             - Main[%d] - Other[%d]"),Main.bAllowJoinInProgress, Other.bAllowJoinInProgress);
	UE_LOG(LogTemp, Warning, TEXT("bIsLANMatch                      - Main[%d] - Other[%d]"),Main.bIsLANMatch, Other.bIsLANMatch);
	UE_LOG(LogTemp, Warning, TEXT("bIsDedicated                     - Main[%d] - Other[%d]"),Main.bIsDedicated, Other.bIsDedicated);
	UE_LOG(LogTemp, Warning, TEXT("bUsesStats                       - Main[%d] - Other[%d]"),Main.bUsesStats, Other.bUsesStats);
	UE_LOG(LogTemp, Warning, TEXT("bAllowInvites                    - Main[%d] - Other[%d]"),Main.bAllowInvites, Other.bAllowInvites);
	UE_LOG(LogTemp, Warning, TEXT("bUsesPresence                    - Main[%d] - Other[%d]"),Main.bUsesPresence, Other.bUsesPresence);
	UE_LOG(LogTemp, Warning, TEXT("bAllowJoinViaPresence            - Main[%d] - Other[%d]"),Main.bAllowJoinViaPresence, Other.bAllowJoinViaPresence);
	UE_LOG(LogTemp, Warning, TEXT("bAllowJoinViaPresenceFriendsOnly - Main[%d] - Other[%d]"),Main.bAllowJoinViaPresenceFriendsOnly, Other.bAllowJoinViaPresenceFriendsOnly);
	UE_LOG(LogTemp, Warning, TEXT("bAntiCheatProtected              - Main[%d] - Other[%d]"),Main.bAntiCheatProtected, Other.bAntiCheatProtected);
	UE_LOG(LogTemp, Warning, TEXT("bUseLobbiesIfAvailable           - Main[%d] - Other[%d]"),Main.bUseLobbiesIfAvailable, Other.bUseLobbiesIfAvailable);
	UE_LOG(LogTemp, Warning, TEXT("bUseLobbiesVoiceChatIfAvailable  - Main[%d] - Other[%d]"),Main.bUseLobbiesVoiceChatIfAvailable, Other.bUseLobbiesVoiceChatIfAvailable);
}

//---------------------------------------------------------------INITIALIZATION------------------------------------------

FNamedOnlineSession* FOnlineSessionASH::AddNamedSession(FName SessionName, const FOnlineSessionSettings& SessionSettings)
{
	FScopeLock ScopeLock(&SessionLock);
	CurrentSession = new FNamedOnlineSession(SessionName, SessionSettings);
    return CurrentSession;
}

FNamedOnlineSession* FOnlineSessionASH::AddNamedSession(FName SessionName, const FOnlineSession& Session)
{
	FScopeLock ScopeLock(&SessionLock);
	CurrentSession = new FNamedOnlineSession(SessionName, Session.SessionSettings);
    return CurrentSession;
}

FUniqueNetIdPtr FOnlineSessionASH::CreateSessionIdFromString(const FString& SessionIdStr)
{
    return FUniqueNetIdPtr();
}

FNamedOnlineSession* FOnlineSessionASH::GetNamedSession(FName SessionName)
{
	return CurrentSession;
}

void FOnlineSessionASH::RemoveNamedSession(FName SessionName)
{
	FScopeLock ScopeLock(&SessionLock);
	delete CurrentSession;
	CurrentSession = nullptr;
}

bool FOnlineSessionASH::HasPresenceSession()
{
    return false;
}

EOnlineSessionState::Type FOnlineSessionASH::GetSessionState(FName SessionName) const
{
    return EOnlineSessionState::Type();
}
//---------------------------------------------------------------INITIALIZATION------------------------------------------



//---------------------------------------------------------------PERIODIC TASKS------------------------------------------

void FOnlineSessionASH::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Session_Interface);
	TickLanTasks(DeltaTime);
	TickPendingInvites(DeltaTime);
}

void FOnlineSessionASH::TickLanTasks(float DeltaTime)
{
	if (CurrentLanSession != nullptr && CurrentLanSession->GetBeaconState() > ELanBeaconState::NotUsingLanBeacon)
	{
		CurrentLanSession->Tick(DeltaTime);
	}
}

void FOnlineSessionASH::TickPendingInvites(float DeltaTime)
{
	/*if (PendingInvite.PendingInviteType == ESteamSession::None)
	{
		return;
	}

	if (OnSessionUserInviteAcceptedDelegates.IsBound())
	{
		FOnlineAsyncItem* NewEvent = nullptr;
		if (PendingInvite.PendingInviteType == ESteamSession::LobbySession)
		{
			NewEvent = new FOnlineAsyncEventSteamLobbyInviteAccepted(SteamSubsystem, *FUniqueNetIdSteam::EmptyId(), *PendingInvite.LobbyId);
		}
		else
		{
			NewEvent = new FOnlineAsyncEventSteamInviteAccepted(SteamSubsystem, *FUniqueNetIdSteam::EmptyId(), PendingInvite.ServerIp);
		}

		if (NewEvent)
		{
			UE_LOG(OnlineSessionASH, Display, TEXT("%s"), *NewEvent->ToString());
			SteamSubsystem->QueueAsyncOutgoingItem(NewEvent);
		}

		// Clear the invite
		PendingInvite.PendingInviteType = EASHSession::None;
	}
	*/
}
//---------------------------------------------------------------PERIODIC TASKS------------------------------------------



//---------------------------------------------------------------CREATING SESSION------------------------------------------
bool FOnlineSessionASH::CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{
    UE_LOG(LogTemp, Error, TEXT("____________________________________________CREATE SESSION___________________________"));
	uint32 Result = ONLINE_FAIL;

	// Check for an existing session
	FNamedOnlineSession* Session = GetNamedSession(SessionName);

	if (Session != nullptr)
	{
		UE_LOG(OnlineSessionASH, Warning, TEXT("Cannot create session '%s': session already exists."), *SessionName.ToString());
		return false;
	}
	
	// Create a new session and deep copy the game settings
	Session = AddNamedSession(SessionName, NewSessionSettings);
	check(Session);
	Session->SessionState = EOnlineSessionState::Creating;
	Session->NumOpenPrivateConnections = NewSessionSettings.NumPrivateConnections;
	Session->NumOpenPublicConnections = NewSessionSettings.bIsDedicated ? NewSessionSettings.NumPublicConnections : NewSessionSettings.NumPublicConnections - 1;

	Session->HostingPlayerNum = HostingPlayerNum;
	Session->OwningUserId = SteamUser() ? FUniqueNetIdASHPtr(FUniqueNetIdASH::Create(SteamUser()->GetSteamID())) : nullptr;
	Session->OwningUserName = SteamFriends() ? UTF8_TO_TCHAR(SteamFriends()->GetPersonaName()) : GetDedicatedServerName();

	// Unique identifier of this build for compatibility
	Session->SessionSettings.BuildUniqueId = GetBuildUniqueId();

	CheckSettings(NewSessionSettings, Session->SessionSettings);

	if (!NewSessionSettings.bIsLANMatch)
	{
		if (Session->SessionSettings.bUseLobbiesIfAvailable)
		{
			//Result = CreateLobbySession(HostingPlayerNum, Session);
			Result = ONLINE_IO_PENDING;
		}
		else
		{
			//Result = CreateInternetSession(HostingPlayerNum, Session);
		}
	}
	else
	{
		Result = CreateLANSession(HostingPlayerNum, Session);
	}

	Session->SessionState = EOnlineSessionState::Pending;

	if (Result == ONLINE_FAIL)
	{
		// Clean up the session info so we don't get into a confused state
		RemoveNamedSession(SessionName);
	}
	if(Result == ONLINE_SUCCESS)
	{
		//RegisterLocalPlayers(Session);
	}
	if (Result != ONLINE_IO_PENDING)
	{
		TriggerOnCreateSessionCompleteDelegates(SessionName, (Result == ONLINE_SUCCESS) ? true : false);
	}

	return Result == ONLINE_IO_PENDING || Result == ONLINE_SUCCESS;
}

bool FOnlineSessionASH::CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{
    // todo: use proper	HostingPlayerId
    return CreateSession(0, SessionName, NewSessionSettings);
}

uint32 FOnlineSessionASH::CreateLANSession(int32 HostingPlayerNum, FNamedOnlineSession* Session)
{
	UE_LOG(OnlineSessionASH, Display, TEXT("Creating LAN session..."));
	check(Session);
	uint32 Result = ONLINE_FAIL;

	// Setup the host session info
	FOnlineSessionInfoASH* NewSessionInfo = new FOnlineSessionInfoASH(EASHSession::LANSession);
	NewSessionInfo->InitLAN();

	if (!Session->OwningUserId.IsValid())
	{
		// Use the lan user id, requires us to advertise the host ip and port
		Session->OwningUserId = FUniqueNetIdASH::Create(k_steamIDLanModeGS);
	}
	Session->SessionInfo = MakeShareable(NewSessionInfo);

	// Don't create the LAN beacon if advertising is off
	if (!Session->SessionSettings.bShouldAdvertise)
	{
		UE_LOG(OnlineSessionASH, Warning, TEXT("Session [%s] wont host properly witout advertisement"), *Session->SessionName.ToString());
		return Result;
	}

	if (CurrentLanSession == nullptr)
	{
		CurrentLanSession = new FLANSession();
	}
	
	FOnValidQueryPacketDelegate QueryPacketDelegate = FOnValidQueryPacketDelegate::CreateRaw(this, &FOnlineSessionASH::OnValidQueryPacketReceived);
	if (CurrentLanSession->Host(QueryPacketDelegate))
	{
		Result = ONLINE_SUCCESS;
	}

	return Result;
}
//---------------------------------------------------------------CREATING SESSION------------------------------------------




//---------------------------------------------------------------PACKET TRANSFER------------------------------------------

void FOnlineSessionASH::OnValidQueryPacketReceived(uint8* PacketData, int32 PacketLength, uint64 ClientNonce)
{
	
	// Iterate through all registered sessions and respond for each LAN match
	FScopeLock ScopeLock(&SessionLock);

	const FOnlineSessionSettings& Settings = CurrentSession->SessionSettings;

	const bool bIsMatchInProgress = CurrentSession->SessionState == EOnlineSessionState::InProgress;

	const bool bIsMatchJoinable = Settings.bIsLANMatch &&
		(!bIsMatchInProgress || Settings.bAllowJoinInProgress) &&
		Settings.NumPublicConnections > 0;

	// Don't respond to query if the session is not a joinable LAN match.
	if (bIsMatchJoinable)
	{
		FNboSerializeToBufferASH Packet(LAN_BEACON_MAX_PACKET_SIZE);
		// Create the basic header before appending additional information
		CurrentLanSession->CreateHostResponsePacket(Packet, ClientNonce);

		// Add all the session details
		AppendSessionToPacket(Packet, CurrentSession);

		// Broadcast this response so the client can see us
		CurrentLanSession->BroadcastPacket(Packet, Packet.GetByteCount());
	}
}

void FOnlineSessionASH::AppendSessionToPacket(FNboSerializeToBufferASH& Packet, FOnlineSession* Session)
{
	/** Owner of the session */
	((FNboSerializeToBuffer&)Packet) << StaticCastSharedPtr<const FUniqueNetIdASH>(Session->OwningUserId)->GetUniqueNetId()
		<< Session->OwningUserName
		<< Session->NumOpenPrivateConnections
		<< Session->NumOpenPublicConnections;

	// Write host info (host addr, session id, and key)
	Packet << *StaticCastSharedPtr<FOnlineSessionInfoASH>(Session->SessionInfo);

	// Now append per game settings
	AppendSessionSettingsToPacket(Packet, &Session->SessionSettings);
}

void FOnlineSessionASH::AppendSessionSettingsToPacket(FNboSerializeToBufferASH& Packet, FOnlineSessionSettings* SessionSettings)
{
#if DEBUG_LAN_BEACON
	UE_LOG(OnlineSessionASH, Display, TEXT("Sending session settings to client"));
#endif 

	// Members of the session settings class
	((FNboSerializeToBuffer&)Packet) << SessionSettings->NumPublicConnections
		<< SessionSettings->NumPrivateConnections
		<< (uint8)SessionSettings->bShouldAdvertise
		<< (uint8)SessionSettings->bIsLANMatch
		<< (uint8)SessionSettings->bIsDedicated
		<< (uint8)SessionSettings->bUsesStats
		<< (uint8)SessionSettings->bAllowJoinInProgress
		<< (uint8)SessionSettings->bAllowInvites
		<< (uint8)SessionSettings->bUsesPresence
		<< (uint8)SessionSettings->bAllowJoinViaPresence
		<< (uint8)SessionSettings->bAllowJoinViaPresenceFriendsOnly
		<< (uint8)SessionSettings->bAntiCheatProtected
		<< SessionSettings->BuildUniqueId;

	// First count number of advertised keys
	int32 NumAdvertisedProperties = 0;
	for (FSessionSettings::TConstIterator It(SessionSettings->Settings); It; ++It)
	{
		const FOnlineSessionSetting& Setting = It.Value();
		if (Setting.AdvertisementType >= EOnlineDataAdvertisementType::ViaOnlineService)
		{
			NumAdvertisedProperties++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Appending %d settings to packet"), NumAdvertisedProperties);

	// Add count of advertised keys and the data
	((FNboSerializeToBuffer&)Packet) << (int32)NumAdvertisedProperties;
	for (FSessionSettings::TConstIterator It(SessionSettings->Settings); It; ++It)
	{
		const FOnlineSessionSetting& Setting = It.Value();
		if (Setting.AdvertisementType >= EOnlineDataAdvertisementType::ViaOnlineService)
		{
			((FNboSerializeToBuffer&)Packet) << It.Key();
			Packet << Setting;
#if DEBUG_LAN_BEACON
			UE_LOG(OnlineSessionASH, Display, TEXT("%s"), *Setting.ToString());
#endif
		}
	}
}

void FOnlineSessionASH::ReadSessionFromPacket(FNboSerializeFromBufferASH& Packet, FOnlineSession* Session)
{
#if DEBUG_LAN_BEACON
	UE_LOG(OnlineSessionASH, Display, TEXT("Reading session information from server"));
#endif

	uint64 OwningUserId;
	Packet >> OwningUserId
		>> Session->OwningUserName
		>> Session->NumOpenPrivateConnections
		>> Session->NumOpenPublicConnections;

	Session->OwningUserId = FUniqueNetIdASH::Create(OwningUserId);

	// Allocate and read the connection data
	FOnlineSessionInfoASH* SteamSessionInfo = new FOnlineSessionInfoASH(EASHSession::LANSession);
	SteamSessionInfo->HostAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Packet >> *SteamSessionInfo;
	Session->SessionInfo = MakeShareable(SteamSessionInfo);

	// Read any per object data using the server object
	ReadSettingsFromPacket(Packet, Session->SessionSettings);
}

void FOnlineSessionASH::ReadSettingsFromPacket(FNboSerializeFromBufferASH& Packet, FOnlineSessionSettings& SessionSettings)
{
#if DEBUG_LAN_BEACON
	UE_LOG(OnlineSessionASH, Display, TEXT("Reading game settings from server"));
#endif

	// Clear out any old settings
	SessionSettings.Settings.Empty();

	// Members of the session settings class
	Packet >> SessionSettings.NumPublicConnections
		>> SessionSettings.NumPrivateConnections;
	uint8 Read = 0;
	// Read all the bools as bytes
	Packet >> Read;
	SessionSettings.bShouldAdvertise = !!Read;
	Packet >> Read;
	SessionSettings.bIsLANMatch = !!Read;
	Packet >> Read;
	SessionSettings.bIsDedicated = !!Read;
	Packet >> Read;
	SessionSettings.bUsesStats = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinInProgress = !!Read;
	Packet >> Read;
	SessionSettings.bAllowInvites = !!Read;
	Packet >> Read;
	SessionSettings.bUsesPresence = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinViaPresence = !!Read;
	Packet >> Read;
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = !!Read;
	Packet >> Read;
	SessionSettings.bAntiCheatProtected = !!Read;

	// BuildId
	Packet >> SessionSettings.BuildUniqueId;

	// Now read the contexts and properties from the settings class
	int32 NumAdvertisedProperties = 0;
	// First, read the number of advertised properties involved, so we can presize the array
	Packet >> NumAdvertisedProperties;
	UE_LOG(LogTemp, Warning, TEXT("Reading %d settings from packet"), NumAdvertisedProperties);
	if (Packet.HasOverflow() == false)
	{
		FName Key;
		// Now read each context individually
		for (int32 Index = 0; Index < NumAdvertisedProperties && Packet.HasOverflow() == false; Index++)
		{
			FOnlineSessionSetting Setting;
			Packet >> Key;
			Packet >> Setting;
			SessionSettings.Set(Key, Setting);

#if DEBUG_LAN_BEACON
			UE_LOG(OnlineSessionASH, Display, TEXT("%s"), *Setting->ToString());
#endif
		}
	}

	// If there was an overflow, treat the string settings/properties as broken
	if (Packet.HasOverflow())
	{
		SessionSettings.Settings.Empty();
		UE_LOG(OnlineSessionASH, Display, TEXT("Packet overflow detected in ReadGameSettingsFromPacket()"));
	}
}

void FOnlineSessionASH::OnValidResponsePacketReceived(uint8* PacketData, int32 PacketLength)
{
	
	// Create an object that we'll copy the data to
	FOnlineSessionSettings NewServer;
	if (CurrentSessionSearch.IsValid() == false)
	{
		UE_LOG(OnlineSessionASH, Warning, TEXT("Failed to create new online game settings object"));
		return;
	}
	
	// Add space in the search results array
	FOnlineSessionSearchResult* NewResult = new (CurrentSessionSearch->SearchResults) FOnlineSessionSearchResult();
	FOnlineSession* NewSession = &NewResult->Session;

	// Prepare to read data from the packet
	FNboSerializeFromBufferASH Packet(PacketData, PacketLength);

	ReadSessionFromPacket(Packet, NewSession);

	// NOTE: we don't notify until the timeout happens
	
}
//---------------------------------------------------------------PACKET TRANSFER------------------------------------------


bool FOnlineSessionASH::StartSession(FName SessionName)
{
    return false;
}

bool FOnlineSessionASH::UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData)
{
    return false;
}

bool FOnlineSessionASH::EndSession(FName SessionName)
{
    return false;
}

bool FOnlineSessionASH::DestroySession(FName SessionName, const FOnDestroySessionCompleteDelegate& CompletionDelegate)
{
    return false;
}

bool FOnlineSessionASH::IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId)
{
    return false;
}

bool FOnlineSessionASH::StartMatchmaking(const TArray<FUniqueNetIdRef>& LocalPlayers, FName SessionName, const FOnlineSessionSettings& NewSessionSettings, TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
    return false;
}

bool FOnlineSessionASH::CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName)
{
    return false;
}

bool FOnlineSessionASH::CancelMatchmaking(const FUniqueNetId& SearchingPlayerId, FName SessionName)
{
    return false;
}


//---------------------------------------------------------------FIND SESSIONS------------------------------------------

bool FOnlineSessionASH::FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	UE_LOG(LogTemp, Warning, TEXT("____________________________________________FIND SESSION___________________________"));
	uint32 Result = ONLINE_FAIL;

	// Dedicated servers shouldn't be matchmaking. 
	if (Subsystem->IsDedicated())
	{
		UE_LOG(OnlineSessionASH, Warning, TEXT("Ignoring game search request  - Dedicated servers shouldn't be matchmaking"));
		SearchSettings->SearchState = EOnlineAsyncTaskState::Failed;
		TriggerOnFindSessionsCompleteDelegates(false);
		return false;
	}

	// Don't start another search while one is in progress
	if (CurrentSessionSearch.IsValid() || SearchSettings->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		UE_LOG(OnlineSessionASH, Warning, TEXT("Ignoring game search request while one is pending"));
		return true;
	}

	// Free up previous results
	SearchSettings->SearchResults.Empty();

	// Copy the search pointer so we can keep it around
	CurrentSessionSearch = SearchSettings;

	// Check if its a LAN query
	if (SearchSettings->bIsLanQuery == false)
	{
		UE_LOG(OnlineSessionASH, Display, TEXT("Running Internet session search query..."));
		//Result = FindInternetSession(SearchSettings);
	}
	else
	{
		Result = FindLANSession(SearchSettings);
	}

	if (Result == ONLINE_IO_PENDING)
	{
		SearchSettings->SearchState = EOnlineAsyncTaskState::InProgress;
	}

	return Result == ONLINE_SUCCESS || Result == ONLINE_IO_PENDING;
}

bool FOnlineSessionASH::FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	// @todo: use proper SearchingPlayerId
	return FindSessions(0, SearchSettings);
}

bool FOnlineSessionASH::FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId, const FUniqueNetId& FriendId, const FOnSingleSessionResultCompleteDelegate& CompletionDelegate)
{
	FOnlineSessionSearchResult EmptyResult;
	UE_LOG(OnlineSessionASH, Warning, TEXT("Ignoring game search request by ID, because it doesnt make sence with Steam's system and is not implemented"));
	CompletionDelegate.ExecuteIfBound(0, false, EmptyResult);
	return true;
}

uint32 FOnlineSessionASH::FindLANSession(const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
	UE_LOG(OnlineSessionASH, Display, TEXT("Running LAN session search query..."));
	uint32 Return = ONLINE_IO_PENDING;

	if (!CurrentLanSession)
	{
		CurrentLanSession = new FLANSession();
	}

	// Recreate the unique identifier for this client
	GenerateNonce((uint8*)&CurrentLanSession->LanNonce, 8);

	FOnValidResponsePacketDelegate ResponseDelegate = FOnValidResponsePacketDelegate::CreateRaw(this, &FOnlineSessionASH::OnValidResponsePacketReceived);
	FOnSearchingTimeoutDelegate TimeoutDelegate = FOnSearchingTimeoutDelegate::CreateRaw(this, &FOnlineSessionASH::OnLANSearchTimeout);

	FNboSerializeToBufferASH Packet(LAN_BEACON_MAX_PACKET_SIZE);
	CurrentLanSession->CreateClientQueryPacket(Packet, CurrentLanSession->LanNonce);
	if (Packet.HasOverflow() || CurrentLanSession->Search(Packet, ResponseDelegate, TimeoutDelegate) == false)
	{
		Return = ONLINE_FAIL;
		delete CurrentLanSession;
		CurrentLanSession = nullptr;

		CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Failed;

		// Just trigger the delegate as having failed
		TriggerOnFindSessionsCompleteDelegates(false);
	}

	return Return;
}

void FOnlineSessionASH::OnLANSearchTimeout()
{
	// See if there were any sessions that were marked as hosting before the search started
	// 
	// I think the comented secsion is entirely fu***ng useless

	/*bool bWasHosting = false;
	{
		FScopeLock ScopeLock(&SessionLock);
		bWasHosting = CurrentSession->SessionSettings.bShouldAdvertise && 
					  CurrentSession->SessionSettings.bIsLANMatch && 
					  Subsystem->IsServer();		
	}

	if (bWasHosting)
	{
		FOnValidQueryPacketDelegate QueryPacketDelegate = FOnValidQueryPacketDelegate::CreateRaw(this, &FOnlineSessionASH::OnValidQueryPacketReceived);
		// Maintain lan beacon if there was a session that was marked as hosting
		if (CurrentLanSession->Host(QueryPacketDelegate))
		{
			UE_LOG(OnlineSessionASH, Warning, TEXT("Failed to restart hosted LAN session after search completion"));
		}
	}
	else
	{
		// Stop future timeouts since we aren't searching any more
		CurrentLanSession->StopLANSession();
	}*/

	if (CurrentSessionSearch.IsValid())
	{
		if (CurrentSessionSearch->SearchResults.Num() > 0)
		{
			// Allow game code to sort the servers
			CurrentSessionSearch->SortSearchResults();
		}
		CurrentSessionSearch->SearchState = EOnlineAsyncTaskState::Done; //setting the given search result

		CurrentSessionSearch = nullptr; //setting the local pointer to the given result as null in order to evade errors
	}

	// Trigger the delegate as complete
	TriggerOnFindSessionsCompleteDelegates(true);
}

bool FOnlineSessionASH::CancelFindSessions()
{
    return false;
}
//---------------------------------------------------------------FIND SESSIONS------------------------------------------



bool FOnlineSessionASH::PingSearchResults(const FOnlineSessionSearchResult& SearchResult)
{
    return false;
}

bool FOnlineSessionASH::JoinSession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession)
{
    return false;
}

bool FOnlineSessionASH::JoinSession(const FUniqueNetId& LocalUserId, FName SessionName, const FOnlineSessionSearchResult& DesiredSession)
{
    return false;
}

bool FOnlineSessionASH::FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend)
{
    return false;
}

bool FOnlineSessionASH::FindFriendSession(const FUniqueNetId& LocalUserId, const FUniqueNetId& Friend)
{
    return false;
}

bool FOnlineSessionASH::FindFriendSession(const FUniqueNetId& LocalUserId, const TArray<FUniqueNetIdRef>& FriendList)
{
    return false;
}

bool FOnlineSessionASH::SendSessionInviteToFriend(int32 LocalUserNum, FName SessionName, const FUniqueNetId& Friend)
{
    return false;
}

bool FOnlineSessionASH::SendSessionInviteToFriend(const FUniqueNetId& LocalUserId, FName SessionName, const FUniqueNetId& Friend)
{
    return false;
}

bool FOnlineSessionASH::SendSessionInviteToFriends(int32 LocalUserNum, FName SessionName, const TArray<FUniqueNetIdRef>& Friends)
{
    return false;
}

bool FOnlineSessionASH::SendSessionInviteToFriends(const FUniqueNetId& LocalUserId, FName SessionName, const TArray<FUniqueNetIdRef>& Friends)
{
    return false;
}

bool FOnlineSessionASH::GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType)
{
    return false;
}

bool FOnlineSessionASH::GetResolvedConnectString(const FOnlineSessionSearchResult& SearchResult, FName PortType, FString& ConnectInfo)
{
    return false;
}

FOnlineSessionSettings* FOnlineSessionASH::GetSessionSettings(FName SessionName)
{
    return nullptr;
}

bool FOnlineSessionASH::RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited)
{
    return false;
}

bool FOnlineSessionASH::RegisterPlayers(FName SessionName, const TArray<FUniqueNetIdRef>& Players, bool bWasInvited)
{
    return false;
}

bool FOnlineSessionASH::UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId)
{
    return false;
}

bool FOnlineSessionASH::UnregisterPlayers(FName SessionName, const TArray<FUniqueNetIdRef>& Players)
{
    return false;
}

void FOnlineSessionASH::RegisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnRegisterLocalPlayerCompleteDelegate& Delegate)
{
}

void FOnlineSessionASH::UnregisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnUnregisterLocalPlayerCompleteDelegate& Delegate)
{
}

int32 FOnlineSessionASH::GetNumSessions()
{
    return int32();
}

void FOnlineSessionASH::DumpSessionState()
{
}

FString FOnlineSessionASH::GetDedicatedServerName() const
{
	FString Prefix = "ASH_Server_", Setting;

	GConfig->GetString(TEXT("OnlineSubsystemASH"), TEXT("DedicatedServerName"), Setting, GEngineIni);
	
	FString ServerName = Prefix + Setting;

	if (ServerName.Len() >= k_cbMaxGameServerName)
	{
		UE_LOG(OnlineSessionASH, Warning, TEXT("DedicatedServerName overflows the maximum amount of characters %d allowed, truncating."), k_cbMaxGameServerName);
		// Must have space for the null terminator
		ServerName.LeftInline(k_cbMaxGameServerName - 1);
	}

	return ServerName;
}
