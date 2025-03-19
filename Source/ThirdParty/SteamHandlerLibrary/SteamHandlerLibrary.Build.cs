// Copyright(c) 2025, Aslume Entertainment. All rights reserved.

using System.IO;
using UnrealBuildTool;

public class SteamHandlerLibrary : ModuleRules
{
	public SteamHandlerLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		PublicSystemIncludePaths.Add("$(ModuleDir)/Public");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "RedistributableBin", "win64", "steam_api64.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("steam_api64.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/SteamHandlerLibrary/RedistributableBin/Win64/steam_api64.dll");
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "RedistributableBin", "osx", "libsteam_api.dylib"));
			//RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/RabotiLibrary/Mac/Release/libExampleLibrary.dylib");
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			//string ExampleSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "RabotiLibrary", "Linux", "x86_64-unknown-linux-gnu", "libExampleLibrary.so");
			//PublicAdditionalLibraries.Add(ExampleSoPath);
			//PublicDelayLoadDLLs.Add(ExampleSoPath);
			//RuntimeDependencies.Add(ExampleSoPath);
		}
	}
}