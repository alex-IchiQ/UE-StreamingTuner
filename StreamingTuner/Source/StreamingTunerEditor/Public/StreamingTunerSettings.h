// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "StreamingTunerProfile.h"
#include "StreamingTunerSettings.generated.h"

/** One row of the per-platform table: which platform, and which profile to write for it. */
USTRUCT()
struct FStreamingTunerPlatformProfile
{
	GENERATED_BODY()

	/** Platform ini name, e.g. Windows, Mac, Linux, Android, IOS, PS5, XSX, Switch. */
	UPROPERTY(EditDefaultsOnly, Category = "Streaming Tuner")
	FName Platform;

	/** Profile whose values are written into this platform's *Engine.ini. */
	UPROPERTY(EditDefaultsOnly, Category = "Streaming Tuner")
	TObjectPtr<UStreamingTunerProfile> Profile = nullptr;
};

/**
 * Quick-start profiles. Selecting anything other than Custom fills the budget
 * (and the locked Settings block) with a baseline.
 * Select Custom to unlock the Settings block and edit every variable by hand.
 */
UENUM()
enum class EStreamingTunerPreset : uint8
{
	// Manual control; the Settings block is unlocked.
	Custom			UMETA(DisplayName = "Custom"),

	// Continuous traversal: keep streaming up while hiding hitches.
	OpenWorld		UMETA(DisplayName = "Open World"),

	// Gated/corridor levels: performance first, blocking acceptable.
	LinearLevels	UMETA(DisplayName = "Linear Levels"),

	// Maximise streaming throughput (engine defaults).
	FastLoad		UMETA(DisplayName = "Fast Load"),
};

/**
 * Project Settings page (Plugins > Streaming Tuner).
 *
 * The top controls (preset, budget) are the quick way to drive
 * everything and are always editable. The Settings block exposes the actual
 * console variables the tuner writes; it is locked unless the preset is Custom.
 * Every change applies live in the editor and is written to
 * Config/DefaultEngine.ini's [ConsoleVariables] section so it persists in builds.
 */
UCLASS(DefaultConfig, Config = Game)
class UStreamingTunerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UStreamingTunerSettings();

	static UStreamingTunerSettings* Get();

	/** Re-derives values from the active preset (if any) and applies everything. */
	void InitialiseAndApply();
	
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FText GetSectionText() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// ================= Quick controls (always editable) =================

	/** Choose a ready-made profile, or Custom to unlock the Settings block below and edit each console variable by hand. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Streaming Tuner")
	EStreamingTunerPreset Preset = EStreamingTunerPreset::Custom;

	/** How much game-thread time per frame goes to level streaming. Low favours performance: work is spread thinly across frames, so spikes are smaller but levels finish loading more slowly. High favours streaming speed, ramping up to Unreal's defaults, at the cost of bigger per-frame spikes. Drives the five time/granularity fields in the Settings block. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Streaming Tuner", meta = (DisplayName = "Streaming Budget", ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0, Delta = 0.05))
	float StreamingBudget = 0.0f;

	/** Sets every field to its current engine default, so the tuner stops changing the engine, and writes those values to ini. */
	void ResetToEngineDefaults();

	/** Removes the tuner's lines from [ConsoleVariables] in DefaultEngine.ini. Use before removing the plugin for a clean uninstall; takes effect after the next launch. */
	void ClearIniEntries();

	// ================= Settings (locked unless Preset == Custom) =================

	/** s.AsyncLoadingTimeLimit - max milliseconds per frame the async loader may spend on the game thread. Lower = cheaper per frame but slower streaming. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Async Loading Time Limit", EditCondition = "Preset == EStreamingTunerPreset::Custom", ClampMin = 0.0, UIMin = 0.0, ForceUnits="ms"))
	float AsyncLoadingTimeLimit = 0.5f;

	/** s.LevelStreamingActorsUpdateTimeLimit - max milliseconds per frame spent updating actors of streaming levels. Lower spreads the work over more frames (cheaper spikes, slower visible completion). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Level Streaming Actors Update Time Limit", EditCondition = "Preset == EStreamingTunerPreset::Custom", ClampMin = 0.0, UIMin = 0.0, ForceUnits="ms"))
	float ActorsUpdateTimeLimit = 0.5f;

	/** s.LevelStreamingComponentsRegistrationGranularity - how many components are registered per batch when a level streams in. Lower = smaller batches = smoother frames but more frames to finish. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Components Registration Granularity", EditCondition = "Preset == EStreamingTunerPreset::Custom", ClampMin = 1.0, UIMin = 1.0))
	float RegistrationGranularity = 1.0f;

	/** s.UnregisterComponentsTimeLimit - max milliseconds per frame spent unregistering components when a level streams out. Lower softens unload spikes. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Unregister Components Time Limit", EditCondition = "Preset == EStreamingTunerPreset::Custom", ClampMin = 0.0, UIMin = 0.0, ForceUnits="ms"))
	float UnregisterComponentsTimeLimit = 0.5f;

	/** s.LevelStreamingComponentsUnregistrationGranularity - how many components are unregistered per batch on level stream-out. Lower = smaller batches = smoother unload. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Components Unregistration Granularity", EditCondition = "Preset == EStreamingTunerPreset::Custom", ClampMin = 1.0, UIMin = 1.0))
	float UnregistrationGranularity = 1.0f;

	/** wp.Runtime.MaxLoadingLevelStreamingCells - cap on how many World Partition cells may load simultaneously. Lower spreads loads out (smoother frames, slower fill-in). 0 means unlimited (engine default). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Max Loading Level Streaming Cells", EditCondition = "Preset == EStreamingTunerPreset::Custom", ClampMin = 0.0, UIMin = 0.0))
	float MaxLoadingLevelStreamingCells = 0.0f;

	/** s.UseUnifiedTimeBudgetForStreaming (UE 5.6+) - checked shares one frame time budget between async asset loading and level streaming; unused time on one side is donated to the other. Often a net win on streaming-heavy projects. Unchecked keeps the separate legacy budgets. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Use Unified Time Budget For Streaming", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	bool bUseUnifiedTimeBudgetForStreaming = false;

	/** s.ForceGCAfterLevelStreamedOut - checked forces a garbage-collection pass after a level streams out (engine default, can cause hitches). Unchecked disables it. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Force GC After Level Streamed Out", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	bool bForceGCAfterLevelStreamedOut = false;

	/** s.ContinuouslyIncrementalGCWhileLevelsPendingPurge - checked keeps running incremental GC while levels wait to be purged (engine default). Unchecked disables it. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Incremental GC While Levels Pending Purge", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	bool bIncrementalGCWhilePurging = false;

	/** wp.Runtime.BlockOnSlowStreaming - checked lets World Partition stall the game thread when the player outruns streaming (engine default). Unchecked lets the player see HLODs instead of freezing. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Block On Slow Streaming", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	bool bBlockOnSlowStreaming = false;

	/** wp.Runtime.BlockOnSlowStreamingRatio - distance ratio at which blocking kicks in. Engine default is 0.25. -1 effectively disables blocking (used with Block On Slow Streaming unchecked). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings",
		meta = (DisplayName = "Block On Slow Streaming Ratio", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	float BlockOnSlowStreamingRatio = -1.0f;

	// ---- Settings > Server: dedicated-server streaming. ----

	/** wp.Runtime.EnableServerStreaming (5.1+) - checked lets a dedicated server stream cells around player streaming sources instead of keeping the whole map loaded. Big CPU/memory win on large multiplayer maps. Unchecked is the engine default. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings|Server",
		meta = (DisplayName = "Enable Server Streaming", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	bool bEnableServerStreaming = false;

	/** wp.Runtime.EnableServerStreamingOut - checked also lets the server unload cells no streaming source needs anymore. Pairs with Enable Server Streaming. Unchecked keeps cells resident once loaded. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings|Server",
		meta = (DisplayName = "Enable Server Streaming Out", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	bool bEnableServerStreamingOut = false;

	/** wp.Runtime.HLOD.ForceDisable - checked skips HLOD generation/loading. On a dedicated server that only needs collision and gameplay, disabling HLOD saves memory and CPU. Leave unchecked on clients. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Settings|Server",
		meta = (DisplayName = "Force Disable HLOD", EditCondition = "Preset == EStreamingTunerPreset::Custom"))
	bool bForceDisableHLOD = false;

	// ================= Profiles (reusable saved configurations) =================

	/** A saved profile asset to load from or save to. Create one via Content Browser > Data Asset > Streaming Tuner Profile. */
	UPROPERTY(EditDefaultsOnly, Category = "Profiles", meta = (DisplayName = "Profile Asset"))
	TObjectPtr<UStreamingTunerProfile> Profile = nullptr;

	/** Copies the selected profile's values into the Settings block above and applies them (switches to Custom). */
	void LoadFromProfile();

	/** Copies the current Settings values into the selected profile asset (remember to save the asset afterwards). */
	void SaveToProfile();

	// ================= Per-Platform overrides =================

	/** Assign a profile per platform. The base values above go to DefaultEngine.ini; each row here is written to that platform's *Engine.ini so it only applies there. */
	UPROPERTY(EditDefaultsOnly, Category = "Platform", meta = (DisplayName = "Platform Profiles", TitleProperty = "Platform"))
	TArray<FStreamingTunerPlatformProfile> PlatformProfiles;

	/** Writes each row's profile into Config/<Platform>/<Platform>Engine.ini's [ConsoleVariables] section. */
	void WritePerPlatformConfigs();

private:
	/** Reads the current Settings fields into a values struct. */
	FStreamingTunerValues GatherValues() const;

	/** Writes a values struct back into the Settings fields. */
	void ApplyValues(const FStreamingTunerValues& In);

	/** Rewrites the five time/granularity fields from a budget t in [0..1]. */
	void ApplyBudgetToScaledFields(float Budget);

	/** Fills the quick controls and the Settings block from a profile's baseline. */
	void ApplyPreset(EStreamingTunerPreset InPreset);

	/** Pushes the current Settings fields to the engine and persists them to ini. */
	void Apply(bool bNotify = false, bool bPersist = true);

	/** Sets a CVar live (if present) and queues it for the ini write. */
	void StageCVar(const FString& Name, float Value);

	/** Writes all staged CVars to [ConsoleVariables] in DefaultEngine.ini. */
	void FlushToIni();

	/** Live engine default for a CVar, or Fallback if it is not registered. */
	static float EngineDefaultOf(const FString& CVarName, float Fallback);

	// Name -> value buffer filled during Apply(), flushed once at the end.
	TMap<FString, float> PendingIni;
};
