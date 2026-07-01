// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Brushes/SlateImageBrush.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FStreamingTunerStyle::StyleInstance = nullptr;

void FStreamingTunerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FStreamingTunerStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		StyleInstance.Reset();
	}
}

FName FStreamingTunerStyle::GetStyleSetName()
{
	static FName Name(TEXT("StreamingTunerStyle"));
	return Name;
}

TSharedRef<FSlateStyleSet> FStreamingTunerStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShared<FSlateStyleSet>(GetStyleSetName());

	if (const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("StreamingTuner")))
	{
		Style->SetContentRoot(Plugin->GetBaseDir() / TEXT("Resources"));
	}

	const FVector2D Icon16(16.0f, 16.0f);
	const FVector2D Icon64(64.0f, 64.0f);

	Style->Set("ClassThumbnail.StreamingTunerProfile", new FSlateImageBrush(Style->RootToContentDir(TEXT("ProfileThumbnail"), TEXT(".png")), Icon64));
	Style->Set("ClassIcon.StreamingTunerProfile", new FSlateImageBrush(Style->RootToContentDir(TEXT("ProfileIcon"), TEXT(".png")), Icon16));

	return Style;
}
