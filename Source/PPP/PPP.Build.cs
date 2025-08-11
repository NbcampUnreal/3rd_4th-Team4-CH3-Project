// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;    // by Team4 (yeoul)

public class PPP : ModuleRules
{
	public PPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "UMG",
            "Slate",
            "SlateCore"
        });

		PrivateDependencyModuleNames.AddRange(new string[]
        {
            "AnimGraphRuntime"
        });

        // GameOver 캐릭터 델리게이트 위해 추가함
        // by Team4 (yeoul)
        PublicIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Characters"),
            Path.Combine(ModuleDirectory, "OutGame"),
            Path.Combine(ModuleDirectory, "Weapons"),
            Path.Combine(ModuleDirectory, "InGame"),
        });
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
