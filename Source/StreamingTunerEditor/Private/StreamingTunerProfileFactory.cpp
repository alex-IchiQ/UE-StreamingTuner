// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerProfileFactory.h"
#include "StreamingTunerProfile.h"

UStreamingTunerProfileFactory::UStreamingTunerProfileFactory()
{
	SupportedClass = UStreamingTunerProfile::StaticClass();
	bCreateNew     = true;
	bEditAfterNew  = true;
}

UObject* UStreamingTunerProfileFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* /*Context*/, FFeedbackContext* /*Warn*/)
{
	return NewObject<UStreamingTunerProfile>(InParent, InClass, InName, Flags);
}
