// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerEditor.h"

#include "StreamingTunerSettings.h"
#include "StreamingTunerSettingsCustomization.h"
#include "StreamingTunerStyle.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FStreamingTunerEditorModule"

void FStreamingTunerEditorModule::StartupModule()
{
	// Custom class icon + thumbnail brushes for the profile asset.
	FStreamingTunerStyle::Initialize();

	// Details customization for the settings page (category order, buttons, refresh).
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(UStreamingTunerSettings::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FStreamingTunerSettingsCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	// Loaded PostEngineInit, so the streaming CVars already exist: push settings.
	UStreamingTunerSettings::Get()->InitialiseAndApply();

	// Note: the asset's name/color/category come from UAssetDefinition_StreamingTunerProfile,
}

void FStreamingTunerEditorModule::ShutdownModule()
{
	FStreamingTunerStyle::Shutdown();

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(UStreamingTunerSettings::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStreamingTunerEditorModule, StreamingTunerEditor)
