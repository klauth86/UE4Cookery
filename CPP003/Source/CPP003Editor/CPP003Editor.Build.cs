using UnrealBuildTool;
using System.IO;

public class CPP003Editor : ModuleRules {
    
	public CPP003Editor(ReadOnlyTargetRules Target) : base(Target) {

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// ... add public include paths required here ...
		PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Public") });

		// ... add other private include paths required here ...
		PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Private") });

		// ... add other public dependencies that you statically link with here ...
		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		// ... add private dependencies that you statically link with here ...	
		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "CPP003", "Engine" });

		// ... add any modules that your module loads dynamically here ...
		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}