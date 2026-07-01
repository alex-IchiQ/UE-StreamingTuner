// Copyright (c) 2026 Aleksey Karpov (IchiQ). All Rights Reserved.

#include "StreamingTunerAssetDefinition.h"
#include "StreamingTunerProfile.h"

#define LOCTEXT_NAMESPACE "StreamingTuner"

FText UAssetDefinition_StreamingTunerProfile::GetAssetDisplayName() const
{
	return LOCTEXT("ProfileAssetName", "Streaming Tuner Profile");
}

FLinearColor UAssetDefinition_StreamingTunerProfile::GetAssetColor() const
{
	return FLinearColor(FColor(45, 212, 191));
}

TSoftClassPtr<UObject> UAssetDefinition_StreamingTunerProfile::GetAssetClass() const
{
	return UStreamingTunerProfile::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_StreamingTunerProfile::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath> Categories = {
		FAssetCategoryPath(LOCTEXT("StreamingTunerCategory", "Streaming Tuner"))
	};
	
	return Categories;
}

#undef LOCTEXT_NAMESPACE
