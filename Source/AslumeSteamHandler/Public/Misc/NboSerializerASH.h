// Copyright (c) 2025, Aslume Entertainment. All rights reserved.

#pragma once

#include "Foundation/OnlineSubsystemASHTypes.h"
#include "NboSerializerOSS.h"

/**
 * Serializes data in network byte order form into a buffer
 */
class FNboSerializeToBufferASH : public FNboSerializeToBufferOSS
{
public:
	/** Default constructor zeros num bytes*/
	FNboSerializeToBufferASH() :
		FNboSerializeToBufferOSS(512)
	{
	}

	/** Constructor specifying the size to use */
	FNboSerializeToBufferASH(uint32 Size) :
		FNboSerializeToBufferOSS(Size)
	{
	}

	/**
	 * Adds Steam session info to the buffer
	 */
	friend inline FNboSerializeToBufferASH& operator<<(FNboSerializeToBufferASH& Ar, const FOnlineSessionInfoASH& SessionInfo)
	{
		check(SessionInfo.HostAddr.IsValid());
		// Skip SessionType (assigned at creation)
		((FNboSerializeToBuffer&)Ar) << *SessionInfo.HostAddr;
		((FNboSerializeToBuffer&)Ar) << SessionInfo.SessionId->GetUniqueNetId();
		return Ar;
	}
};

/**
 * Class used to write data into packets for sending via system link
 */
class FNboSerializeFromBufferASH : public FNboSerializeFromBufferOSS
{
public:
	/**
	 * Initializes the buffer, size, and zeros the read offset
	 */
	FNboSerializeFromBufferASH(uint8* Packet, int32 Length) :
		FNboSerializeFromBufferOSS(Packet, Length)
	{
	}

	/**
	 * Reads Steam session info from the buffer
	 */
	friend inline FNboSerializeFromBufferASH& operator>>(FNboSerializeFromBufferASH& Ar, FOnlineSessionInfoASH& SessionInfo)
	{
		check(SessionInfo.HostAddr.IsValid());

		// Skip SessionType (assigned at creation)
		Ar >> *SessionInfo.HostAddr;

		uint64 SessionId;
		Ar >> SessionId;
		SessionInfo.SessionId = FUniqueNetIdASH::Create(SessionId);

		return Ar;
	}
};