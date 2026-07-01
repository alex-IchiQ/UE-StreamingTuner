// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StreamingTunerNames.h"
#include "StreamingTunerValues.generated.h"

/** Dedicated-server streaming values (shown as a nested group in the details panel). */
USTRUCT(BlueprintType)
struct FStreamingTunerServerValues
{
	GENERATED_BODY()

	/** wp.Runtime.EnableServerStreaming - stream cells on a dedicated server instead of keeping the whole map resident. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	bool bEnableServerStreaming = false;

	/** wp.Runtime.EnableServerStreamingOut - also let the server unload cells no streaming source needs. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	bool bEnableServerStreamingOut = false;

	/** wp.Runtime.HLOD.ForceDisable - skip HLOD generation/loading (server memory/CPU saving). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	bool bForceDisableHLOD = false;
};

/**
 * The full set of console-variable values the tuner drives, as plain data.
 * Shared by profile data assets and per-platform overrides; the settings page
 * keeps its own individual fields and converts to/from this struct so the field
 * list lives in one place.
 */
USTRUCT(BlueprintType)
struct FStreamingTunerValues
{
	GENERATED_BODY()

	/** s.AsyncLoadingTimeLimit (ms). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming", meta = (ClampMin = 0.0, UIMin = 0.0))
	float AsyncLoadingTimeLimit = 0.5f;

	/** s.LevelStreamingActorsUpdateTimeLimit (ms). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming", meta = (ClampMin = 0.0, UIMin = 0.0))
	float ActorsUpdateTimeLimit = 0.5f;

	/** s.LevelStreamingComponentsRegistrationGranularity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming", meta = (ClampMin = 1.0, UIMin = 1.0))
	float RegistrationGranularity = 1.0f;

	/** s.UnregisterComponentsTimeLimit (ms). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming", meta = (ClampMin = 0.0, UIMin = 0.0))
	float UnregisterComponentsTimeLimit = 0.5f;

	/** s.LevelStreamingComponentsUnregistrationGranularity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming", meta = (ClampMin = 1.0, UIMin = 1.0))
	float UnregistrationGranularity = 1.0f;

	/** s.UseUnifiedTimeBudgetForStreaming (UE 5.6+). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming")
	bool bUseUnifiedTimeBudgetForStreaming = false;

	/** wp.Runtime.MaxLoadingLevelStreamingCells (0 = unlimited). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming", meta = (ClampMin = 0.0, UIMin = 0.0))
	float MaxLoadingLevelStreamingCells = 0.0f;

	/** s.ForceGCAfterLevelStreamedOut. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming")
	bool bForceGCAfterLevelStreamedOut = false;

	/** s.ContinuouslyIncrementalGCWhileLevelsPendingPurge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming")
	bool bIncrementalGCWhilePurging = false;

	/** wp.Runtime.BlockOnSlowStreaming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming")
	bool bBlockOnSlowStreaming = false;

	/** wp.Runtime.BlockOnSlowStreamingRatio (-1 disables blocking). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming")
	float BlockOnSlowStreamingRatio = -1.0f;

	/** Dedicated-server streaming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Streaming")
	FStreamingTunerServerValues Server;

	/** Console-variable name/value pairs in apply order (bools become 1/0). */
	TArray<TPair<FString, float>> ToCVarPairs() const
	{
		return {
			{ StreamingTuner::CVar_AsyncLoadingTimeLimit,         AsyncLoadingTimeLimit },
			{ StreamingTuner::CVar_ActorsUpdateTimeLimit,         ActorsUpdateTimeLimit },
			{ StreamingTuner::CVar_RegistrationGranularity,       RegistrationGranularity },
			{ StreamingTuner::CVar_UnregisterComponentsTimeLimit, UnregisterComponentsTimeLimit },
			{ StreamingTuner::CVar_UnregistrationGranularity,     UnregistrationGranularity },
			{ StreamingTuner::CVar_UnifiedTimeBudget,             bUseUnifiedTimeBudgetForStreaming ? 1.f : 0.f },
			{ StreamingTuner::CVar_MaxLoadingCells,               MaxLoadingLevelStreamingCells },
			{ StreamingTuner::CVar_ForceGCAfterLevelStreamedOut,  bForceGCAfterLevelStreamedOut ? 1.f : 0.f },
			{ StreamingTuner::CVar_IncrementalGCWhilePurging,      bIncrementalGCWhilePurging ? 1.f : 0.f },
			{ StreamingTuner::CVar_BlockOnSlowStreaming,           bBlockOnSlowStreaming ? 1.f : 0.f },
			{ StreamingTuner::CVar_BlockOnSlowStreamingRatio,      BlockOnSlowStreamingRatio },
			{ StreamingTuner::CVar_EnableServerStreaming,          Server.bEnableServerStreaming ? 1.f : 0.f },
			{ StreamingTuner::CVar_EnableServerStreamingOut,       Server.bEnableServerStreamingOut ? 1.f : 0.f },
			{ StreamingTuner::CVar_HLODForceDisable,               Server.bForceDisableHLOD ? 1.f : 0.f },
		};
	}
};
