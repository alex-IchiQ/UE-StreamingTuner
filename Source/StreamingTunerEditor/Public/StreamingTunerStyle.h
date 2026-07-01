// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSlateStyleSet;

/** Registers the custom class icon and thumbnail brushes for the profile asset. */
class FStreamingTunerStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static FName GetStyleSetName();

private:
	static TSharedRef<FSlateStyleSet> Create();
	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
