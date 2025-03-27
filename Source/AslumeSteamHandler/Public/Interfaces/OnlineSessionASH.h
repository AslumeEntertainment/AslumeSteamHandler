// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once 

#include "Interfaces/OnlineSessionInterface.h"
//#include "Online/LANBeacon.h"

class FNamedOnlineSession;
class FOnlineSubsystemASH;
class FLANSession;

DECLARE_LOG_CATEGORY_EXTERN(OnlineSessionASH, Log, All);

/**
 * Interface definition for the online services session services
 * Session services are defined as anything related managing a session
 * and its state within a platform service
 */
class FOnlineSessionASH : public IOnlineSession
{
private:
	/** Critical sections for thread safe operation of session lists */
	mutable FCriticalSection SessionLock;

	/** Current session settings */
	FNamedOnlineSession* CurrentSession = nullptr;
	FOnlineSubsystemASH* Subsystem = nullptr;

	FLANSession* CurrentLanSession =  nullptr;

	FOnlineSessionASH() {}

protected:
	void TickLanTasks(float DeltaTime);
	void TickPendingInvites(float DeltaTime);

public:

	FOnlineSessionASH(FOnlineSubsystemASH* InSubsystem) : Subsystem(InSubsystem) {}

	void Tick(float DeltaTime);

	// Inherited via IOnlineSession
	FNamedOnlineSession* AddNamedSession(FName SessionName, const FOnlineSessionSettings& SessionSettings) override;
	FNamedOnlineSession* AddNamedSession(FName SessionName, const FOnlineSession& Session) override;
	FUniqueNetIdPtr CreateSessionIdFromString(const FString& SessionIdStr) override;
	FNamedOnlineSession* GetNamedSession(FName SessionName) override;
	void RemoveNamedSession(FName SessionName) override;
	bool HasPresenceSession() override;
	EOnlineSessionState::Type GetSessionState(FName SessionName) const override;

	bool CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) override;
	bool CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) override;
	uint32 CreateLANSession(int32 HostingPlayerNum, FNamedOnlineSession* Session);

	void OnValidQueryPacketReceived(uint8* PacketData, int32 PacketLength, uint64 ClientNonce);
	void AppendSessionToPacket(class FNboSerializeToBufferASH& Packet, class FOnlineSession* Session);
	void AppendSessionSettingsToPacket(FNboSerializeToBufferASH& Packet, FOnlineSessionSettings* SessionSettings);

	bool StartSession(FName SessionName) override;
	bool UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData) override;
	bool EndSession(FName SessionName) override;
	bool DestroySession(FName SessionName, const FOnDestroySessionCompleteDelegate& CompletionDelegate) override;
	bool IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId) override;
	bool StartMatchmaking(const TArray<FUniqueNetIdRef>& LocalPlayers, FName SessionName, const FOnlineSessionSettings& NewSessionSettings, TSharedRef<FOnlineSessionSearch>& SearchSettings) override;
	bool CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName) override;
	bool CancelMatchmaking(const FUniqueNetId& SearchingPlayerId, FName SessionName) override;
	bool FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings) override;
	bool FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings) override;
	bool FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId, const FUniqueNetId& FriendId, const FOnSingleSessionResultCompleteDelegate& CompletionDelegate) override;
	bool CancelFindSessions() override;
	bool PingSearchResults(const FOnlineSessionSearchResult& SearchResult) override;
	bool JoinSession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) override;
	bool JoinSession(const FUniqueNetId& LocalUserId, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) override;
	bool FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend) override;
	bool FindFriendSession(const FUniqueNetId& LocalUserId, const FUniqueNetId& Friend) override;
	bool FindFriendSession(const FUniqueNetId& LocalUserId, const TArray<FUniqueNetIdRef>& FriendList) override;
	bool SendSessionInviteToFriend(int32 LocalUserNum, FName SessionName, const FUniqueNetId& Friend) override;
	bool SendSessionInviteToFriend(const FUniqueNetId& LocalUserId, FName SessionName, const FUniqueNetId& Friend) override;
	bool SendSessionInviteToFriends(int32 LocalUserNum, FName SessionName, const TArray<FUniqueNetIdRef>& Friends) override;
	bool SendSessionInviteToFriends(const FUniqueNetId& LocalUserId, FName SessionName, const TArray<FUniqueNetIdRef>& Friends) override;
	bool GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType) override;
	bool GetResolvedConnectString(const FOnlineSessionSearchResult& SearchResult, FName PortType, FString& ConnectInfo) override;
	FOnlineSessionSettings* GetSessionSettings(FName SessionName) override;
	bool RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited) override;
	bool RegisterPlayers(FName SessionName, const TArray<FUniqueNetIdRef>& Players, bool bWasInvited) override;
	bool UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId) override;
	bool UnregisterPlayers(FName SessionName, const TArray<FUniqueNetIdRef>& Players) override;
	void RegisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnRegisterLocalPlayerCompleteDelegate& Delegate) override;
	void UnregisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnUnregisterLocalPlayerCompleteDelegate& Delegate) override;
	int32 GetNumSessions() override;
	void DumpSessionState() override;


	FString GetDedicatedServerName() const;
};