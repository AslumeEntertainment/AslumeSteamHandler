#include "Foundation/Interfaces/OnlineSessionASH.h"

FNamedOnlineSession* FOnlineSessionASH::AddNamedSession(FName SessionName, const FOnlineSessionSettings& SessionSettings)
{
    return nullptr;
}

FNamedOnlineSession* FOnlineSessionASH::AddNamedSession(FName SessionName, const FOnlineSession& Session)
{
    return nullptr;
}

FUniqueNetIdPtr FOnlineSessionASH::CreateSessionIdFromString(const FString& SessionIdStr)
{
    return FUniqueNetIdPtr();
}

FNamedOnlineSession* FOnlineSessionASH::GetNamedSession(FName SessionName)
{
    return nullptr;
}

void FOnlineSessionASH::RemoveNamedSession(FName SessionName)
{
}

bool FOnlineSessionASH::HasPresenceSession()
{
    return false;
}

EOnlineSessionState::Type FOnlineSessionASH::GetSessionState(FName SessionName) const
{
    return EOnlineSessionState::Type();
}

bool FOnlineSessionASH::CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{
    UE_LOG(LogTemp, Error, TEXT("____________________________________________CREATE SESSION___________________________"));
    return false;
}

bool FOnlineSessionASH::CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings)
{
    // todo: use proper	HostingPlayerId
    return CreateSession(0, SessionName, NewSessionSettings);
}

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

bool FOnlineSessionASH::FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
    return false;
}

bool FOnlineSessionASH::FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings)
{
    return false;
}

bool FOnlineSessionASH::FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId, const FUniqueNetId& FriendId, const FOnSingleSessionResultCompleteDelegate& CompletionDelegate)
{
    return false;
}

bool FOnlineSessionASH::CancelFindSessions()
{
    return false;
}

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
