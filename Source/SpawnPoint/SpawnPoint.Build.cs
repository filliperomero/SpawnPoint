// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpawnPoint : ModuleRules
{
	public SpawnPoint(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"GameplayTags", 
			"PhysicsCore",
			"UMG",
			"Slate",
			"SlateCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput"
		});
	}
}
