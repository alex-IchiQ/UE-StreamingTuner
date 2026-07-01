// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "StreamingTunerAssetDefinition.generated.h"

/**
 * Modern asset definition (UE 5.2+) for the profile: sets its content-browser
 * display name, color and category. Auto-registered by the engine - no manual
 * registration needed. The class icon/thumbnail come from FStreamingTunerStyle.
 */
UCLASS()
class UAssetDefinition_StreamingTunerProfile : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	virtual FText GetAssetDisplayName() const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
};
