// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CPP008 : ModuleRules
{
	public CPP008(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "SlateCore", "Slate", "UMG" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
