// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StreamingTunerValues.h"
#include "StreamingTunerProfile.generated.h"

/**
 * A saved, named set of Streaming Tuner values. Create these from the content
 * browser (Streaming Tuner > Streaming Tuner Profile) to keep
 * reusable streaming profiles in your project and assign them per platform
 * in Project Settings > Game > Streaming Tuner.
 */
UCLASS(BlueprintType)
class STREAMINGTUNERRUNTIME_API UStreamingTunerProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Free-text note shown in the settings page (e.g. "open world, perf-leaning"). */
	UPROPERTY(EditAnywhere, Category = "Streaming Tuner", meta = (MultiLine = true))
	FString Description;

	/** The values applied when this profile is loaded or assigned to a platform. */
	UPROPERTY(EditAnywhere, Category = "Streaming Tuner")
	FStreamingTunerValues Values;
};
