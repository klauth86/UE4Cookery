// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPP012 : ModuleRules
{
	public CPP012(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "Slate", "SlateCore" });
	}
}
