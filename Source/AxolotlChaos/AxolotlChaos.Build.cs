// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AxolotlChaos : ModuleRules
{
	public AxolotlChaos(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
