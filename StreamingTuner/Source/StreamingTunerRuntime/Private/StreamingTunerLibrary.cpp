// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerLibrary.h"
#include "StreamingTunerNames.h"
#include "HAL/IConsoleManager.h"

DEFINE_LOG_CATEGORY(LogStreamingTuner);

namespace
{
	// Holds the tuned values while the tuner is paused, so Resume can restore them.
	// Only meaningful between a Pause and the matching Resume.
	TMap<FString, float>& GetPauseCache()
	{
		static TMap<FString, float> Cache;
		return Cache;
	}
}

void UStreamingTunerLibrary::PauseTuner()
{
#if !WITH_EDITOR
	TMap<FString, float>& Cache = GetPauseCache();
	Cache.Reset();

	for (const FString& Name : StreamingTuner::GetManagedCVarNames())
	{
		if (IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(*Name))
		{
			Cache.Add(Name, Var->GetFloat());
			Var->Set(*Var->GetDefaultValue());
		}
	}

	UE_LOG(LogStreamingTuner, Verbose, TEXT("Streaming Tuner paused (%d cvars)."), Cache.Num());
#endif
}

void UStreamingTunerLibrary::ResumeTuner()
{
#if !WITH_EDITOR
	const TMap<FString, float>& Cache = GetPauseCache();

	for (const TPair<FString, float>& Entry : Cache)
	{
		if (IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(*Entry.Key))
		{
			Var->Set(Entry.Value);
		}
	}

	UE_LOG(LogStreamingTuner, Verbose, TEXT("Streaming Tuner resumed (%d cvars)."), Cache.Num());
#endif
}
