// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once 

//#include "Interfaces/OnlineSessionDelegates.h"
//#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
//#include "OnlineSubsystemNullPackage.h"
//#include "Online/LANBeacon.h"

/**
 * Interface definition for the online services session services
 * Session services are defined as anything related managing a session
 * and its state within a platform service
 */
class FOnlineSessionASH : public IOnlineSession
{
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
};