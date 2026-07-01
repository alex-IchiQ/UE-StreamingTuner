// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StreamingTunerLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStreamingTuner, Log, All);

/**
 * Blueprint entry points for temporarily turning the tuner off and back on.
 * These only do anything in a packaged (non-editor) build, which is where the
 * tuned CVars are actually persisted.
 */
UCLASS()
class STREAMINGTUNERRUNTIME_API UStreamingTunerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Caches the current values of every managed CVar and restores engine
	 * defaults. Call this before heavy startup work (e.g. shader compilation).
	 */
	UFUNCTION(BlueprintCallable, Category = "Streaming Tuner")
	static void PauseTuner();

	/** Re-applies the values cached by PauseTuner(). */
	UFUNCTION(BlueprintCallable, Category = "Streaming Tuner")
	static void ResumeTuner();
};
