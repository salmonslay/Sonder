// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PROJ : ModuleRules
{
	public PROJ(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] {"AdvancedSessions"});
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"Sockets", 
			"CableComponent",
			"Niagara", 
			"LevelSequence", 
			"MovieScene", 
			"UMG"
		});
		
	}
}
