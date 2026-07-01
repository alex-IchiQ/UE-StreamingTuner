// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

using UnrealBuildTool;

public class StreamingTunerEditor : ModuleRules
{
	public StreamingTunerEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"UnrealEd",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"DeveloperSettings",
			"Slate",
			"SlateCore",
			"PropertyEditor",
			"AssetDefinition",
			"Projects",
			"StreamingTunerRuntime",
		});
	}
}
