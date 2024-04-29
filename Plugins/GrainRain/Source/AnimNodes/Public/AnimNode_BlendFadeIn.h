// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimNodeBase.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "Data/BoneTransforms.h"
#include "AnimNode_BlendFadeIn.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct ANIMNODES_API FAnimNode_BlendFadeIn : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()
	
	// Input link
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeartDub, meta = (PinShownByDefault))
	FBoneTransforms PoseTransforms;


	
	FAnimNode_BlendFadeIn();
	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void UpdateComponentPose_AnyThread(const FAnimationUpdateContext& Context) override;
	//virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	// End of FAnimNode_Base interface

	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	
};