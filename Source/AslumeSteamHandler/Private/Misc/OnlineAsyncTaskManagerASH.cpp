#include "Misc/OnlineAsyncTaskManagerASH.h"

//#include "Foundation/SubsystemDefinition.h"

void FOnlineAsyncTaskManagerASH::OnlineTick()
{
	check(ASHSubsystem);
	check(FPlatformTLS::GetCurrentThreadId() == OnlineThreadId);

	//if (ASHSubsystem->IsSteamClientAvailable())
	//{
	SteamAPI_RunCallbacks();
	//}

	//if (ASHSubsystem->IsSteamServerAvailable())
	//{
	//	SteamGameServer_RunCallbacks();
	//}
}