// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEAdvancedUI : ModuleRules
{
	public UEAdvancedUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"JsEnv", 
			"Puerts",
			"GameplayTags",
			"UMG",           // 添加UMG模块，用于UWidget、UUserWidget等UI类
			"Slate",         // 添加Slate模块
			"SlateCore"      // 添加SlateCore模块
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
