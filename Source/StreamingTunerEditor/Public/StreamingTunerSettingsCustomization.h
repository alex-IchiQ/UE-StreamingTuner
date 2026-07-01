// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

/**
 * Orders the Streaming Tuner categories, places the action buttons at the
 * bottom of their categories, and refreshes the panel after a button runs.
 */
class FStreamingTunerSettingsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
