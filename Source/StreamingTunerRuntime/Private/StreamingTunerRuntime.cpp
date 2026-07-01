// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerRuntime.h"

#if !WITH_EDITOR
#include "StreamingTunerLibrary.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "PipelineStateCache.h"
#include "ShaderPipelineCache.h"
#endif

#define LOCTEXT_NAMESPACE "FStreamingTunerRuntimeModule"

void FStreamingTunerRuntimeModule::StartupModule()
{
#if !WITH_EDITOR
	PostWorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddRaw(
		this, &FStreamingTunerRuntimeModule::HandlePostWorldInit);
#endif
}

void FStreamingTunerRuntimeModule::ShutdownModule()
{
#if !WITH_EDITOR
	FWorldDelegates::OnPostWorldInitialization.Remove(PostWorldInitHandle);
#endif
}

#if !WITH_EDITOR
void FStreamingTunerRuntimeModule::HandlePostWorldInit(UWorld* World, const UWorld::InitializationValues)
{
	if (!World || World->WorldType != EWorldType::Game)
	{
		return;
	}

	// If a previous world left a timer running, clear it and restore tuned values first.
	if (PollTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(PollTimerHandle);
		PollTimerHandle.Invalidate();
		UStreamingTunerLibrary::ResumeTuner();
	}

	IdleSeconds = 0;
	UStreamingTunerLibrary::PauseTuner();

	World->GetTimerManager().SetTimer(
		PollTimerHandle,
		FTimerDelegate::CreateRaw(this, &FStreamingTunerRuntimeModule::PollShaderCompilation, World),
		1.0f, /*loop*/ true);
}

void FStreamingTunerRuntimeModule::PollShaderCompilation(UWorld* World)
{
	if (!IsValid(World))
	{
		return;
	}

	const bool bBusy = PipelineStateCache::IsPrecaching() || FShaderPipelineCache::IsBatchingPaused();
	IdleSeconds = bBusy ? 0 : (IdleSeconds + 1);

	if (IdleSeconds >= RequiredIdleSeconds)
	{
		UStreamingTunerLibrary::ResumeTuner();
		World->GetTimerManager().ClearTimer(PollTimerHandle);
		PollTimerHandle.Invalidate();
	}
}
#endif

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStreamingTunerRuntimeModule, StreamingTunerRuntime)
