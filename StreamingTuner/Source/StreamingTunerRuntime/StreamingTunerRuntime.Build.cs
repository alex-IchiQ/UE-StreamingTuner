// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

using UnrealBuildTool;

public class StreamingTunerRuntime : ModuleRules
{
	public StreamingTunerRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"RHI",
			"RenderCore",
		});
	}
}
