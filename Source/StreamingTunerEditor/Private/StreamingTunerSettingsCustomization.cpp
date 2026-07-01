// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerSettingsCustomization.h"
#include "StreamingTunerSettings.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "IPropertyUtilities.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "StreamingTuner"

TSharedRef<IDetailCustomization> FStreamingTunerSettingsCustomization::MakeInstance()
{
	return MakeShared<FStreamingTunerSettingsCustomization>();
}

namespace
{
	// A single labelled button that runs Action then refreshes the whole panel.
	TSharedRef<SWidget> MakeActionButton(
		const FText& Label,
		TWeakObjectPtr<UStreamingTunerSettings> Settings,
		TSharedRef<IPropertyUtilities> Utils,
		TFunction<void(UStreamingTunerSettings&)> Action)
	{
		return SNew(SButton)
			.ContentPadding(FMargin(10.f, 3.f))
			.OnClicked_Lambda([Settings, Utils, Action]()
			{
				if (UStreamingTunerSettings* S = Settings.Get())
				{
					Action(*S);
				}
				Utils->ForceRefresh();
				return FReply::Handled();
			})
			[
				SNew(STextBlock).Text(Label)
			];
	}
}

void FStreamingTunerSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	TWeakObjectPtr<UStreamingTunerSettings> Settings;
	for (const TWeakObjectPtr<UObject>& Obj : Objects)
	{
		if (UStreamingTunerSettings* S = Cast<UStreamingTunerSettings>(Obj.Get()))
		{
			Settings = S;
			break;
		}
	}
	
	if (!Settings.IsValid())
	{
		return;
	}

	const TSharedRef<IPropertyUtilities> Utils = DetailBuilder.GetPropertyUtilities();

	// --- Category order (lower = higher in the panel) ---
	IDetailCategoryBuilder& Main     = DetailBuilder.EditCategory("Streaming Tuner");
	IDetailCategoryBuilder& SettingsC = DetailBuilder.EditCategory("Settings");
	IDetailCategoryBuilder& Profiles  = DetailBuilder.EditCategory("Profiles");
	IDetailCategoryBuilder& PerPlat   = DetailBuilder.EditCategory("Platform");

	Main.SetSortOrder(1);
	SettingsC.SetSortOrder(2);
	Profiles.SetSortOrder(3);
	PerPlat.SetSortOrder(4);

	// --- Streaming Tuner: Reset / Clear buttons at the bottom ---
	Main.AddCustomRow(LOCTEXT("ActionsFilter", "Actions"))
	.WholeRowContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(0.f, 2.f, 6.f, 2.f)
		[
			MakeActionButton(LOCTEXT("Reset", "Reset To Engine Defaults"), Settings, Utils, [](UStreamingTunerSettings& S) { S.ResetToEngineDefaults(); })
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(0.f, 2.f)
		[
			MakeActionButton(LOCTEXT("Clear", "Clear Plugin Entries From Ini"), Settings, Utils, [](UStreamingTunerSettings& S) { S.ClearIniEntries(); })
		]
	];

	// --- Profiles: Load / Save buttons at the bottom ---
	Profiles.AddCustomRow(LOCTEXT("ProfileActionsFilter", "Profile Actions"))
	.WholeRowContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(0.f, 2.f, 6.f, 2.f)
		[
			MakeActionButton(LOCTEXT("Load", "Load From Profile"), Settings, Utils, [](UStreamingTunerSettings& S) { S.LoadFromProfile(); })
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(0.f, 2.f)
		[
			MakeActionButton(LOCTEXT("Save", "Save To Profile"), Settings, Utils, [](UStreamingTunerSettings& S) { S.SaveToProfile(); })
		]
	];

	// --- Platform: Write button at the bottom ---
	PerPlat.AddCustomRow(LOCTEXT("PerPlatformActionsFilter", "Platform Actions"))
	.WholeRowContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(0.f, 2.f)
		[
			MakeActionButton(LOCTEXT("Write", "Write Platform Configs"), Settings, Utils,
				[](UStreamingTunerSettings& S) { S.WritePerPlatformConfigs(); })
		]
	];
}

#undef LOCTEXT_NAMESPACE
