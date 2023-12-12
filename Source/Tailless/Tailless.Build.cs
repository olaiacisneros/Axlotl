// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tailless : ModuleRules
{
	public Tailless(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
