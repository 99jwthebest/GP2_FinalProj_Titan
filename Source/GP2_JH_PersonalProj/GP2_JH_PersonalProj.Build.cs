// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GP2_JH_PersonalProj : ModuleRules
{
	public GP2_JH_PersonalProj(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
