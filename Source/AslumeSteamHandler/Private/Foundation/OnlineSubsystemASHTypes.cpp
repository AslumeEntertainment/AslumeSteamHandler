// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#include "Foundation/OnlineSubsystemASHTypes.h"

#include "Online/LANBeacon.h"
#include "SocketSubsystem.h"

FOnlineSessionInfoASH::FOnlineSessionInfoASH(EASHSession::Type InSessionType) :
	SessionType(InSessionType),
	ASHP2PAddr(nullptr),
	ConnectionMethod((InSessionType == EASHSession::LANSession) ? FASHConnectionMethod::Direct : FASHConnectionMethod::None),
	SessionId(FUniqueNetIdASH::Create((uint64)0)),
	HostAddr(nullptr)
{
}

FOnlineSessionInfoASH::FOnlineSessionInfoASH(EASHSession::Type InSessionType, const FUniqueNetIdASH& InSessionId) :
	SessionType(InSessionType),
	ASHP2PAddr(nullptr),
	ConnectionMethod(FASHConnectionMethod::None),
	SessionId(InSessionId.AsShared()),
	HostAddr(nullptr)
{
}

void FOnlineSessionInfoASH::Init()
{
}

void FOnlineSessionInfoASH::InitLAN()
{
	SessionType = EASHSession::LANSession;
	ConnectionMethod = FASHConnectionMethod::Direct;

	uint64 Nonce = 0;
	GenerateNonce((uint8*)&Nonce, 8);
	SessionId = FUniqueNetIdASH::Create(Nonce);

	// Read the IP from the system
	bool bCanBindAll = false;
	HostAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
	// Now set the port that was configured
	HostAddr->SetPort(FURL::UrlConfig.DefaultPort);

	Init();
}
