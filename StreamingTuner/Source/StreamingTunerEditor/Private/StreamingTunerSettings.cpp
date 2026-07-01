// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerSettings.h"
#include "StreamingTunerNames.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "Misc/ConfigCacheIni.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "StreamingTuner"

namespace
{
	const TCHAR* GIniSection = TEXT("ConsoleVariables");

	FString EngineIniPath()
	{
		return FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini"));
	}

	// e.g. Android -> <Project>/Config/Android/AndroidEngine.ini
	FString PlatformEngineIniPath(const FName Platform)
	{
		const FString P = Platform.ToString();
		return FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir() / P / (P + TEXT("Engine.ini")));
	}

	void WritePairsToIni(const TArray<TPair<FString, float>>& Pairs, const FString& IniPath)
	{
		if (!GConfig)
		{
			return;
		}
		for (const TPair<FString, float>& Pair : Pairs)
		{
			GConfig->SetFloat(GIniSection, *Pair.Key, Pair.Value, IniPath);
		}
		GConfig->Flush(false, IniPath);
	}

	void ShowToast(const FText& Message, bool bSuccess = true)
	{
		FNotificationInfo Info(Message);
		Info.ExpireDuration = 3.0f;
		Info.bFireAndForget = true;
		Info.bUseSuccessFailIcons = true;
		if (const TSharedPtr<SNotificationItem> Item = FSlateNotificationManager::Get().AddNotification(Info))
		{
			Item->SetCompletionState(bSuccess ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);
		}
	}
}

UStreamingTunerSettings::UStreamingTunerSettings()
{
}

UStreamingTunerSettings* UStreamingTunerSettings::Get()
{
	return GetMutableDefault<UStreamingTunerSettings>();
}

FName UStreamingTunerSettings::GetContainerName() const { return TEXT("Project"); }
FName UStreamingTunerSettings::GetCategoryName()  const { return TEXT("Game"); }
FText UStreamingTunerSettings::GetSectionText()   const { return LOCTEXT("Section", "Streaming Tuner"); }

float UStreamingTunerSettings::EngineDefaultOf(const FString& CVarName, const float Fallback)
{
	if (IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(*CVarName))
	{
		return FCString::Atof(*Var->GetDefaultValue());
	}
	return Fallback;
}

void UStreamingTunerSettings::InitialiseAndApply()
{
	if (Preset != EStreamingTunerPreset::Custom)
	{
		ApplyPreset(Preset);
	}

	Apply(false);
}

void UStreamingTunerSettings::ApplyBudgetToScaledFields(float Budget)
{
	const FVector2D BudgetSpace(0.f, 1.f);
	auto Scale = [&](const FString& Name, const float CheapFloor)
	{
		const float Default = EngineDefaultOf(Name, CheapFloor);
		return FMath::GetMappedRangeValueClamped(BudgetSpace, FVector2D(CheapFloor, Default), Budget);
	};

	AsyncLoadingTimeLimit          = Scale(StreamingTuner::CVar_AsyncLoadingTimeLimit, 0.5f);
	ActorsUpdateTimeLimit          = Scale(StreamingTuner::CVar_ActorsUpdateTimeLimit, 0.5f);
	RegistrationGranularity        = Scale(StreamingTuner::CVar_RegistrationGranularity, 1.0f);
	UnregisterComponentsTimeLimit  = Scale(StreamingTuner::CVar_UnregisterComponentsTimeLimit, 0.5f);
	UnregistrationGranularity      = Scale(StreamingTuner::CVar_UnregistrationGranularity, 1.0f);
}

void UStreamingTunerSettings::ApplyPreset(EStreamingTunerPreset InPreset)
{
	// Each profile is a budget t in [0..1] plus the GC / block intentions.
	float Budget = 0.f;
	
	bool bDisableGC    = true;
	bool bDisableBlock = true;

	switch (InPreset)
	{
	case EStreamingTunerPreset::OpenWorld:
		Budget = 0.5f;  bDisableGC = true;  bDisableBlock = true;  break;
	case EStreamingTunerPreset::LinearLevels:
		Budget = 0.25f; bDisableGC = true;  bDisableBlock = false; break;
	case EStreamingTunerPreset::FastLoad:
		Budget = 1.0f;  bDisableGC = false; bDisableBlock = false; break;
	case EStreamingTunerPreset::Custom:
	default:
		return; // Leave everything as the user left it.
	}

	StreamingBudget = Budget;
	ApplyBudgetToScaledFields(Budget);

	const bool bGCEnabled = !bDisableGC;
	bForceGCAfterLevelStreamedOut = bGCEnabled;
	bIncrementalGCWhilePurging    = bGCEnabled;

	bBlockOnSlowStreaming     = !bDisableBlock;
	BlockOnSlowStreamingRatio = bDisableBlock ? -1.f : EngineDefaultOf(StreamingTuner::CVar_BlockOnSlowStreamingRatio, 0.25f);

	// Left at the engine baseline by presets; tune them in Custom.
	bUseUnifiedTimeBudgetForStreaming = EngineDefaultOf(StreamingTuner::CVar_UnifiedTimeBudget, 0.f) != 0.f;
	MaxLoadingLevelStreamingCells     = EngineDefaultOf(StreamingTuner::CVar_MaxLoadingCells, 0.f);
	// Server fields are independent of profiles and are left untouched.
}

#if WITH_EDITOR
void UStreamingTunerSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const bool bInteractive = (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive) != 0;
	const FName Changed = PropertyChangedEvent.GetPropertyName();

	const FName PresetName = GET_MEMBER_NAME_CHECKED(UStreamingTunerSettings, Preset);
	const FName BudgetName = GET_MEMBER_NAME_CHECKED(UStreamingTunerSettings, StreamingBudget);

	if (Changed == PresetName)
	{
		ApplyPreset(Preset);
	}
	else if (Changed == BudgetName)
	{
		Preset = EStreamingTunerPreset::Custom;
		ApplyBudgetToScaledFields(StreamingBudget);
	}
	// Editing a raw Settings field is only possible in Custom; just apply it.

	// While dragging a slider, update CVars live but don't hit disk or toast on
	// every tick; the final (non-interactive) change persists and notifies once.
	Apply(!bInteractive, !bInteractive);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UStreamingTunerSettings::Apply(bool bNotify, bool bPersist)
{
	PendingIni.Reset();

	for (const TPair<FString, float>& Entry : GatherValues().ToCVarPairs())
	{
		StageCVar(Entry.Key, Entry.Value);
	}

	if (bPersist)
	{
		FlushToIni();
	}

	if (bNotify)
	{
		ShowToast(LOCTEXT("Applied", "Streaming Tuner: settings applied"));
	}
}

void UStreamingTunerSettings::StageCVar(const FString& Name, float Value)
{
	if (IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(*Name))
	{
		Var->Set(Value);
	}
	PendingIni.Add(Name, Value);
}

void UStreamingTunerSettings::FlushToIni()
{
	if (!GConfig)
	{
		return;
	}

	const FString IniPath = EngineIniPath();
	for (const TPair<FString, float>& Entry : PendingIni)
	{
		GConfig->SetFloat(GIniSection, *Entry.Key, Entry.Value, IniPath);
	}
	GConfig->Flush(false, IniPath);
}

void UStreamingTunerSettings::ResetToEngineDefaults()
{
	// Budget 1.0 maps the scaled fields straight onto their engine defaults.
	Preset = EStreamingTunerPreset::Custom;
	StreamingBudget = 1.0f;
	ApplyBudgetToScaledFields(1.0f);

	bUseUnifiedTimeBudgetForStreaming = EngineDefaultOf(StreamingTuner::CVar_UnifiedTimeBudget, 0.f) != 0.f;
	MaxLoadingLevelStreamingCells     = EngineDefaultOf(StreamingTuner::CVar_MaxLoadingCells, 0.f);

	bForceGCAfterLevelStreamedOut = EngineDefaultOf(StreamingTuner::CVar_ForceGCAfterLevelStreamedOut, 1.f) != 0.f;
	bIncrementalGCWhilePurging    = EngineDefaultOf(StreamingTuner::CVar_IncrementalGCWhilePurging, 1.f) != 0.f;

	bBlockOnSlowStreaming     = EngineDefaultOf(StreamingTuner::CVar_BlockOnSlowStreaming, 1.f) != 0.f;
	BlockOnSlowStreamingRatio = EngineDefaultOf(StreamingTuner::CVar_BlockOnSlowStreamingRatio, 0.25f);

	bEnableServerStreaming    = EngineDefaultOf(StreamingTuner::CVar_EnableServerStreaming, 0.f) != 0.f;
	bEnableServerStreamingOut = EngineDefaultOf(StreamingTuner::CVar_EnableServerStreamingOut, 0.f) != 0.f;
	bForceDisableHLOD         = EngineDefaultOf(StreamingTuner::CVar_HLODForceDisable, 0.f) != 0.f;

	Apply(false);
	TryUpdateDefaultConfigFile();
	ShowToast(LOCTEXT("Reset", "Streaming Tuner: reset to engine defaults"));
}

void UStreamingTunerSettings::ClearIniEntries()
{
	int32 Removed = 0;
	if (GConfig)
	{
		const FString IniPath = EngineIniPath();
		for (const FString& Name : StreamingTuner::GetAllCVarNames())
		{
			if (GConfig->DoesSectionExist(GIniSection, IniPath))
			{
				GConfig->RemoveKey(GIniSection, *Name, IniPath);
				++Removed;
			}
		}
		GConfig->Flush(false, IniPath);
	}

	ShowToast(FText::Format(
		LOCTEXT("Cleared", "Streaming Tuner: cleared {0} entries from DefaultEngine.ini (effective next launch)"),
		FText::AsNumber(Removed)));
}

FStreamingTunerValues UStreamingTunerSettings::GatherValues() const
{
	FStreamingTunerValues V;
	V.AsyncLoadingTimeLimit            = AsyncLoadingTimeLimit;
	V.ActorsUpdateTimeLimit            = ActorsUpdateTimeLimit;
	V.RegistrationGranularity          = RegistrationGranularity;
	V.UnregisterComponentsTimeLimit    = UnregisterComponentsTimeLimit;
	V.UnregistrationGranularity        = UnregistrationGranularity;
	V.bUseUnifiedTimeBudgetForStreaming = bUseUnifiedTimeBudgetForStreaming;
	V.MaxLoadingLevelStreamingCells    = MaxLoadingLevelStreamingCells;
	V.bForceGCAfterLevelStreamedOut    = bForceGCAfterLevelStreamedOut;
	V.bIncrementalGCWhilePurging       = bIncrementalGCWhilePurging;
	V.bBlockOnSlowStreaming            = bBlockOnSlowStreaming;
	V.BlockOnSlowStreamingRatio        = BlockOnSlowStreamingRatio;
	V.Server.bEnableServerStreaming    = bEnableServerStreaming;
	V.Server.bEnableServerStreamingOut = bEnableServerStreamingOut;
	V.Server.bForceDisableHLOD         = bForceDisableHLOD;
	return V;
}

void UStreamingTunerSettings::ApplyValues(const FStreamingTunerValues& In)
{
	AsyncLoadingTimeLimit             = In.AsyncLoadingTimeLimit;
	ActorsUpdateTimeLimit             = In.ActorsUpdateTimeLimit;
	RegistrationGranularity           = In.RegistrationGranularity;
	UnregisterComponentsTimeLimit     = In.UnregisterComponentsTimeLimit;
	UnregistrationGranularity         = In.UnregistrationGranularity;
	bUseUnifiedTimeBudgetForStreaming = In.bUseUnifiedTimeBudgetForStreaming;
	MaxLoadingLevelStreamingCells     = In.MaxLoadingLevelStreamingCells;
	bForceGCAfterLevelStreamedOut     = In.bForceGCAfterLevelStreamedOut;
	bIncrementalGCWhilePurging        = In.bIncrementalGCWhilePurging;
	bBlockOnSlowStreaming             = In.bBlockOnSlowStreaming;
	BlockOnSlowStreamingRatio         = In.BlockOnSlowStreamingRatio;
	bEnableServerStreaming            = In.Server.bEnableServerStreaming;
	bEnableServerStreamingOut         = In.Server.bEnableServerStreamingOut;
	bForceDisableHLOD                 = In.Server.bForceDisableHLOD;
}

void UStreamingTunerSettings::LoadFromProfile()
{
	if (!Profile)
	{
		ShowToast(LOCTEXT("NoProfileLoad", "Streaming Tuner: no profile asset selected"), false);
		return;
	}

	Preset = EStreamingTunerPreset::Custom; // unlock the Settings block to show the loaded values
	ApplyValues(Profile->Values);
	Apply(false);
	TryUpdateDefaultConfigFile();

	ShowToast(FText::Format(
		LOCTEXT("Loaded", "Streaming Tuner: loaded profile '{0}'"),
		FText::FromName(Profile->GetFName())));
}

void UStreamingTunerSettings::SaveToProfile()
{
	if (!Profile)
	{
		ShowToast(LOCTEXT("NoProfileSave", "Streaming Tuner: no profile asset selected"), false);
		return;
	}

	Profile->Values = GatherValues();
	(void)Profile->MarkPackageDirty();

	ShowToast(FText::Format(
		LOCTEXT("Saved", "Streaming Tuner: copied settings into '{0}' (save the asset to keep it)"),
		FText::FromName(Profile->GetFName())));
}

void UStreamingTunerSettings::WritePerPlatformConfigs()
{
	int32 Written = 0;
	for (const FStreamingTunerPlatformProfile& Row : PlatformProfiles)
	{
		if (Row.Platform.IsNone() || !Row.Profile)
		{
			continue;
		}
		WritePairsToIni(Row.Profile->Values.ToCVarPairs(), PlatformEngineIniPath(Row.Platform));
		++Written;
	}

	ShowToast(FText::Format(
		LOCTEXT("PerPlatform", "Streaming Tuner: wrote {0} per-platform config(s)"),
		FText::AsNumber(Written)));
}
