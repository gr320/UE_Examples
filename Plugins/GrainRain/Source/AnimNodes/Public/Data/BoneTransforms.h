#pragma once

#include "BoneTransforms.generated.h"


USTRUCT(BlueprintType)
struct FBoneTransforms
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Names;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTransform> Transforms;
};
