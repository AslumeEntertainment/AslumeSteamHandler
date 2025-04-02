// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#include "Foundation/OnlineSubsystemASHTypes.h"

#include "Online/LANBeacon.h"
#include "SocketSubsystem.h"

FOnlineSessionInfoASH::FOnlineSessionInfoASH(EASHSession::Type InSessionType) :
	SessionType(InSessionType),
	SessionId(FUniqueNetIdASH::Create((uint64)0)),
	HostAddr(nullptr),
	ASHP2PAddr(nullptr),
	ConnectionMethod((InSessionType == EASHSession::LANSession) ? FASHConnectionMethod::Direct : FASHConnectionMethod::None)
{
}

FOnlineSessionInfoASH::FOnlineSessionInfoASH(EASHSession::Type InSessionType, const FUniqueNetIdASH& InSessionId) :
	SessionType(InSessionType),
	SessionId(InSessionId.AsShared()),
	HostAddr(nullptr),
	ASHP2PAddr(nullptr),
	ConnectionMethod(FASHConnectionMethod::None)
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

FString LexToString(const FASHConnectionMethod Method)
{
	switch (Method)
	{
	default:
	case FASHConnectionMethod::None:
		return TEXT("None");
	case FASHConnectionMethod::Direct:
		return TEXT("Direct");
	case FASHConnectionMethod::P2P:
		return TEXT("P2P");
	case FASHConnectionMethod::PartnerHosted:
		return TEXT("PartnerHosted");
	}
}

FASHConnectionMethod ToConnectionMethod(const FString& InString)
{
	if (InString == TEXT("Direct"))
	{
		return FASHConnectionMethod::Direct;
	}
	else if (InString == TEXT("P2P"))
	{
		return FASHConnectionMethod::P2P;
	}
	else if (InString == TEXT("PartnerHosted"))
	{
		return FASHConnectionMethod::PartnerHosted;
	}
	else
	{
		return FASHConnectionMethod::None;
	}
}