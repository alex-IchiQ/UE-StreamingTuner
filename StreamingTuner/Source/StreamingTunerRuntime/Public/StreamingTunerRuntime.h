// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"

/**
 * Runtime side of the tuner.
 *
 * In a packaged build the tuned values can make the very first seconds after
 * launch worse, because, while the shader pipeline is still precaching, we want
 * the engine to spend its full budget there. So at world start we temporarily
 * restore engine defaults, then poll once per second; once shader work has been
 * idle for a few seconds, we put the tuned values back.
 */
class FStreamingTunerRuntimeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if !WITH_EDITOR
private:
	void HandlePostWorldInit(UWorld* World, const UWorld::InitializationValues Values);
	void PollShaderCompilation(UWorld* World);

	FDelegateHandle PostWorldInitHandle;
	FTimerHandle    PollTimerHandle;
	int32           IdleSeconds = 0;

	static constexpr int32 RequiredIdleSeconds = 5;
#endif
};
