// Copyright 2023 Christina Piberger. All rights reserved.

using UnrealBuildTool;

public class SimpleRacer : ModuleRules
{
	public SimpleRacer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{ 
			"AIModule",
			"Core",
			"CoreUObject", 
			"Chaos",
			"ChaosVehicles",
            "Engine",
            "EnhancedInput",
            "GameplayTasks",
            "InputCore", 
			"RacingAI",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
		});

        PrivateIncludePaths.Add(ModuleDirectory);
        PublicIncludePaths.Add(ModuleDirectory);
    }
}
