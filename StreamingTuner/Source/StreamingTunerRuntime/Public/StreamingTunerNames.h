// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * One place that lists every console variable the tuner touches.
 * Both the editor settings and the runtime pause/resume logic read from here,
 * so there is a single source of truth for the CVar names.
 */
namespace StreamingTuner
{
	// Variables driven by the budget slider (each has its own min..engineDefault range).
	inline const FString CVar_AsyncLoadingTimeLimit           = TEXT("s.AsyncLoadingTimeLimit");
	inline const FString CVar_ActorsUpdateTimeLimit           = TEXT("s.LevelStreamingActorsUpdateTimeLimit");
	inline const FString CVar_RegistrationGranularity         = TEXT("s.LevelStreamingComponentsRegistrationGranularity");
	inline const FString CVar_UnregisterComponentsTimeLimit   = TEXT("s.UnregisterComponentsTimeLimit");
	inline const FString CVar_UnregistrationGranularity       = TEXT("s.LevelStreamingComponentsUnregistrationGranularity");

	// Unified frame budget shared by async loading and level streaming (UE 5.6+).
	inline const FString CVar_UnifiedTimeBudget               = TEXT("s.UseUnifiedTimeBudgetForStreaming");
	// Cap on how many World Partition cells may load at once.
	inline const FString CVar_MaxLoadingCells                 = TEXT("wp.Runtime.MaxLoadingLevelStreamingCells");

	// Garbage-collection-during-streaming toggle.
	inline const FString CVar_ForceGCAfterLevelStreamedOut    = TEXT("s.ForceGCAfterLevelStreamedOut");
	inline const FString CVar_IncrementalGCWhilePurging       = TEXT("s.ContinuouslyIncrementalGCWhileLevelsPendingPurge");

	// Block-on-slow-streaming toggle (World Partition).
	inline const FString CVar_BlockOnSlowStreaming            = TEXT("wp.Runtime.BlockOnSlowStreaming");
	inline const FString CVar_BlockOnSlowStreamingRatio       = TEXT("wp.Runtime.BlockOnSlowStreamingRatio");

	// Dedicated-server streaming (independent of the per-frame budget).
	inline const FString CVar_EnableServerStreaming           = TEXT("wp.Runtime.EnableServerStreaming");
	inline const FString CVar_EnableServerStreamingOut        = TEXT("wp.Runtime.EnableServerStreamingOut");
	inline const FString CVar_HLODForceDisable                = TEXT("wp.Runtime.HLOD.ForceDisable");

	/** Per-frame budget variables. Managed by pause/resume and written to ini. */
	inline TArray<FString> GetManagedCVarNames()
	{
		return {
			CVar_AsyncLoadingTimeLimit,
			CVar_ActorsUpdateTimeLimit,
			CVar_RegistrationGranularity,
			CVar_UnregisterComponentsTimeLimit,
			CVar_UnregistrationGranularity,
			CVar_UnifiedTimeBudget,
			CVar_MaxLoadingCells,
			CVar_ForceGCAfterLevelStreamedOut,
			CVar_IncrementalGCWhilePurging,
			CVar_BlockOnSlowStreaming,
			CVar_BlockOnSlowStreamingRatio,
		};
	}

	/** Dedicated-server variables. Written to ini but NOT touched by pause/resume. */
	inline TArray<FString> GetServerCVarNames()
	{
		return {
			CVar_EnableServerStreaming,
			CVar_EnableServerStreamingOut,
			CVar_HLODForceDisable,
		};
	}

	/** Everything the tuner writes - used for the read-only engine-default reference. */
	inline TArray<FString> GetAllCVarNames()
	{
		TArray<FString> All = GetManagedCVarNames();
		All.Append(GetServerCVarNames());
		return All;
	}
}
