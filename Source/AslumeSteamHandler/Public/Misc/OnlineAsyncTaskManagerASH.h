// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once

#include "OnlineAsyncTaskManager.h"

#include "Steam/isteamgameserver.h"

#include "CommonIncludes.h"

class FOnlineSubsystemASH;

/**
 * Base class that holds a delegate to fire when a given async task is complete
 */
class FOnlineAsyncTaskASH : public FOnlineAsyncTaskBasic<FOnlineSubsystemASH>
{
//PACKAGE_SCOPE:
private:

	/** Unique handle for the Steam async call initiated */
	SteamAPICall_t CallbackHandle;

	/** Hidden on purpose */
	FOnlineAsyncTaskASH() :
		FOnlineAsyncTaskBasic(nullptr),
		CallbackHandle(k_uAPICallInvalid)
	{
	}

public:

	FOnlineAsyncTaskASH(FOnlineSubsystemASH* InASHSubsystem, SteamAPICall_t InCallbackHandle) :
		FOnlineAsyncTaskBasic(InASHSubsystem),
		CallbackHandle(InCallbackHandle)
	{
	}

	virtual ~FOnlineAsyncTaskASH()
	{
	}
};



/**
 *	Aslume Steam Handler version of the async task manager to register the various Steam callbacks with the engine
 */
class FOnlineAsyncTaskManagerASH : public FOnlineAsyncTaskManager
{
private:

	/** Delegate registered with Steam to trigger when another Steam client makes a P2P call to this instance */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnP2PSessionRequest, P2PSessionRequest_t, OnP2PSessionRequestCallback);
	/** Delegate registered with Steam to trigger when a connection between two steam P2P endpoints fails */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnP2PSessionConnectFail, P2PSessionConnectFail_t, OnP2PSessionConnectFailCallback);
	/** Delegate registered with Steam to trigger when another Steam client makes a P2P call to this instance (gameserver API) */
	STEAM_GAMESERVER_CALLBACK(FOnlineAsyncTaskManagerASH, OnP2PSessionRequestGS, P2PSessionRequest_t, OnP2PSessionRequestGSCallback);
	/** Delegate registered with Steam to trigger when a connection between two steam P2P endpoints fails (gameserver API) */
	STEAM_GAMESERVER_CALLBACK(FOnlineAsyncTaskManagerASH, OnP2PSessionConnectFailGS, P2PSessionConnectFail_t, OnP2PSessionConnectFailGSCallback);

	/** Delegate registered with Steam to trigger when a user (client API) is connected to the Steam servers  (usually don't get this because we're already connected externally) */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnSteamServersConnected, SteamServersConnected_t, OnSteamServersConnectedCallback);
	/** Delegate registered with Steam to trigger when a user (client API) is disconnected from the Steam servers.  All realtime services are disabled until a connect event above */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnSteamServersDisconnected, SteamServersDisconnected_t, OnSteamServersDisconnectedCallback);
	/** Delegate registered with Steam to trigger when a server (gameserver API) is connected to the Steam servers.  Initiated with a LogOnAnonymous() call */
	STEAM_GAMESERVER_CALLBACK(FOnlineAsyncTaskManagerASH, OnSteamServersConnectedGS, SteamServersConnected_t, OnSteamServersConnectedGSCallback);
	/** Delegate registered with Steam to trigger when a server (gameserver API) is disconnected from Steam.  Will occur on DestroySession() */
	STEAM_GAMESERVER_CALLBACK(FOnlineAsyncTaskManagerASH, OnSteamServersDisconnectedGS, SteamServersDisconnected_t, OnSteamServersDisconnectedGSCallback);
	/** Delegate registered with Steam to trigger when a server (gameserver API) connection fails. */
	STEAM_GAMESERVER_CALLBACK(FOnlineAsyncTaskManagerASH, OnSteamServersConnectFailureGS, SteamServerConnectFailure_t, OnSteamServersConnectFailureGSCallback);
	/** Delegate registered with Steam to trigger when a server (gameserver API) is properly connected to Steam servers, logon isn't complete until this occurs */
	STEAM_GAMESERVER_CALLBACK(FOnlineAsyncTaskManagerASH, OnPolicyResponseGS, GSPolicyResponse_t, OnPolicyResponseGSCallback);

	/** Delegate registered with Steam to trigger when a user (client API) gets a response for a GetTicketForWebApiResponse request */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnGetTicketForWebApiResponse, GetTicketForWebApiResponse_t, OnGetTicketForWebApiResponseCallback);
	/** Delegate registered with Steam to trigger when a server (gameserver API) gets a response for a Steam session authentication ticket request */
	STEAM_GAMESERVER_CALLBACK(FOnlineAsyncTaskManagerASH, OnAuthenticationResponseGS, ValidateAuthTicketResponse_t, OnAuthenticationResponseGSCallback);
	/** Delegate registered with Steam to trigger when a user (client API) gets a response for a Steam session authentication ticket request */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnAuthenticationResponse, ValidateAuthTicketResponse_t, OnAuthenticationResponseCallback);

	/** Delegate registered with Steam to trigger when a user accepts an invite request to a non lobby session */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnInviteAccepted, GameRichPresenceJoinRequested_t, OnInviteAcceptedCallback);
	/** Delegate registered with Steam to trigger when a user attempts JIP (via Steam client) or accepts an invite request (via Steam client) to a lobby session */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnLobbyInviteAccepted, GameLobbyJoinRequested_t, OnLobbyInviteAcceptedCallback);
	/** Delegate registered with Steam to trigger when a user joins a lobby (successfully or not) */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnLobbyEnter, LobbyEnter_t, OnLobbyEnterCallback);
	/** Delegate registered with Steam to trigger when a lobby chat update occurs (usually users joining/leaving lobby) */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnLobbyChatUpdate, LobbyChatUpdate_t, OnLobbyChatUpdateCallback);
	/**
	 * Delegate registered with Steam to trigger when a lobby update occurs
	 * As host (or any client joined to the lobby), any call to SetLobbyData will trigger this
	 * As client searching for lobbies, once per call to RequestLobbyData
	 */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnLobbyDataUpdate, LobbyDataUpdate_t, OnLobbyDataUpdateCallback);
	/** Delegate registered with Steam to trigger when the currently logged in user receives their stats from RequestStats() */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnUserStatsReceived, UserStatsReceived_t, OnUserStatsReceivedCallback);
	/** Delegate registered with Steam to trigger when the currently logged in user has their stats stored */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnUserStatsStored, UserStatsStored_t, OnUserStatsStoredCallback);
	/** Delegate registered with Steam to trigger when any previously requested user's stat are unloaded by the system */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnUserStatsUnloaded, UserStatsUnloaded_t, OnUserStatsUnloadedCallback);
	/** Delegate registered with Steam to trigger when the Steam Overlay is activated */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnExternalUITriggered, GameOverlayActivated_t, OnExternalUITriggeredCallback);
	/** Delegate registered with Steam to trigger when Steam is shutting down */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnSteamShutdown, SteamShutdown_t, OnSteamShutdownCallback);
	/** Delegate registered with Steam to trigger when Steam gets updates about user rich presence */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnRichPresenceUpdate, FriendRichPresenceUpdate_t, OnRichPresenceUpdateCallback);
	/** Delegate registered with Steam to trigger when Steam gets updates about friends status */
	STEAM_CALLBACK(FOnlineAsyncTaskManagerASH, OnFriendStatusUpdate, PersonaStateChange_t, OnFriendStatusUpdateCallback);

	//GameServerChangeRequested_t

protected:

	/** Cached reference to the main online subsystem */
	FOnlineSubsystemASH* ASHSubsystem;

public:

	FOnlineAsyncTaskManagerASH(FOnlineSubsystemASH* InOnlineSubsystem) :
		OnP2PSessionRequestCallback(this, &FOnlineAsyncTaskManagerASH::OnP2PSessionRequest),
		OnP2PSessionConnectFailCallback(this, &FOnlineAsyncTaskManagerASH::OnP2PSessionConnectFail),
		OnP2PSessionRequestGSCallback(this, &FOnlineAsyncTaskManagerASH::OnP2PSessionRequestGS),
		OnP2PSessionConnectFailGSCallback(this, &FOnlineAsyncTaskManagerASH::OnP2PSessionConnectFailGS),
		OnSteamServersConnectedCallback(this, &FOnlineAsyncTaskManagerASH::OnSteamServersConnected),
		OnSteamServersDisconnectedCallback(this, &FOnlineAsyncTaskManagerASH::OnSteamServersDisconnected),
		OnSteamServersConnectedGSCallback(this, &FOnlineAsyncTaskManagerASH::OnSteamServersConnectedGS),
		OnSteamServersDisconnectedGSCallback(this, &FOnlineAsyncTaskManagerASH::OnSteamServersDisconnectedGS),
		OnSteamServersConnectFailureGSCallback(this, &FOnlineAsyncTaskManagerASH::OnSteamServersConnectFailureGS),
		OnPolicyResponseGSCallback(this, &FOnlineAsyncTaskManagerASH::OnPolicyResponseGS),
		OnGetTicketForWebApiResponseCallback(this, &FOnlineAsyncTaskManagerASH::OnGetTicketForWebApiResponse),
		OnAuthenticationResponseGSCallback(this, &FOnlineAsyncTaskManagerASH::OnAuthenticationResponseGS),
		OnAuthenticationResponseCallback(this, &FOnlineAsyncTaskManagerASH::OnAuthenticationResponse),
		OnInviteAcceptedCallback(this, &FOnlineAsyncTaskManagerASH::OnInviteAccepted),
		OnLobbyInviteAcceptedCallback(this, &FOnlineAsyncTaskManagerASH::OnLobbyInviteAccepted),
		OnLobbyEnterCallback(this, &FOnlineAsyncTaskManagerASH::OnLobbyEnter),
		OnLobbyChatUpdateCallback(this, &FOnlineAsyncTaskManagerASH::OnLobbyChatUpdate),
		OnLobbyDataUpdateCallback(this, &FOnlineAsyncTaskManagerASH::OnLobbyDataUpdate),
		OnUserStatsReceivedCallback(this, &FOnlineAsyncTaskManagerASH::OnUserStatsReceived),
		OnUserStatsStoredCallback(this, &FOnlineAsyncTaskManagerASH::OnUserStatsStored),
		OnUserStatsUnloadedCallback(this, &FOnlineAsyncTaskManagerASH::OnUserStatsUnloaded),
		OnExternalUITriggeredCallback(this, &FOnlineAsyncTaskManagerASH::OnExternalUITriggered),
		OnSteamShutdownCallback(this, &FOnlineAsyncTaskManagerASH::OnSteamShutdown),
		OnRichPresenceUpdateCallback(this, &FOnlineAsyncTaskManagerASH::OnRichPresenceUpdate),
		OnFriendStatusUpdateCallback(this, &FOnlineAsyncTaskManagerASH::OnFriendStatusUpdate),
		ASHSubsystem(InOnlineSubsystem)
	{
	}

	~FOnlineAsyncTaskManagerASH()
	{
	}

	// FOnlineAsyncTaskManager
	virtual void OnlineTick() override;

	// FOnlineAsyncTaskManagerASH
};