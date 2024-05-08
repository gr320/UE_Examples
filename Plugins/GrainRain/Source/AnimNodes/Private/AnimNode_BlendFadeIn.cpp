#include "AnimNode_BlendFadeIn.h"

#include <iostream>

#include "Animation/AnimInstanceProxy.h"


FAnimNode_BlendFadeIn::FAnimNode_BlendFadeIn()
{
}

void FAnimNode_BlendFadeIn::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	FAnimNode_SkeletalControlBase::CacheBones_AnyThread(Context);
}

void FAnimNode_BlendFadeIn::UpdateComponentPose_AnyThread(const FAnimationUpdateContext& Context)
{
	FAnimNode_SkeletalControlBase::UpdateComponentPose_AnyThread(Context);
}


void FAnimNode_BlendFadeIn::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output,
                                                         TArray<FBoneTransform>& OutBoneTransforms)
{


	if (PoseTransforms.Names.Num() > 0)
	{
		//const FBoneContainer& requiredBones = Output.AnimInstanceProxy->GetRequiredBones();
		const FBoneContainer& boneContainer = Output.Pose.GetPose().GetBoneContainer();
		FTransform componentTransform = Output.AnimInstanceProxy->GetComponentTransform();
		
		//Output.Pose.Empty();
		const FBoneContainer& RequiredBone = Output.AnimInstanceProxy->GetRequiredBones();
		Output.Pose.InitPose(&RequiredBone);

		//UE_LOG(LogTemp, Error, TEXT("[FAnimNode_ApplyBonePose] {EvaluateSkeletalControl_AnyThread} Names:%d"),PoseTransforms.Names.Num());
		for (int i = 0; i < PoseTransforms.Names.Num(); ++i)
		{
			//boneContainer.GetParentBoneIndex()
			FName name = FName(*PoseTransforms.Names[i]);
			FBoneReference boneToModify(name);
			auto ret = boneToModify.Initialize(boneContainer);
			if (!ret)
			{
				//UE_LOG(LogTemp, Error, TEXT("[FAnimNode_ApplyBonePose] {EvaluateSkeletalControl_AnyThread} boneToModify Initialize error:%s"),name);
			}
			else
			{

				
				FCompactPoseBoneIndex compactPoseBoneToModify = boneToModify.GetCompactPoseIndex(boneContainer);
				if (compactPoseBoneToModify != INDEX_NONE && PoseTransforms.Transforms.Num()>i)
				{
					//获取基于SkeletalComponent的Transform信息.
					FTransform newBoneTM = Output.Pose.GetComponentSpaceTransform(compactPoseBoneToModify);
					//转化为BoneSpace 
					//FAnimationRuntime::ConvertCSTransformToBoneSpace(componentTransform, Output.Pose, newBoneTM, compactPoseBoneToModify, /*BCS_ParentBoneSpace*/BCS_ParentBoneSpace);
					//newBoneTM.SetTranslation(PoseTransforms.Transforms[i].GetTranslation());//
				
					newBoneTM.SetRotation(PoseTransforms.Transforms[i].GetRotation());//
					newBoneTM.SetLocation(PoseTransforms.Transforms[i].GetLocation());
					//BoneSpace 转化为SkeletalComponent.
					FAnimationRuntime::ConvertBoneSpaceTransformToCS(componentTransform, Output.Pose, newBoneTM, compactPoseBoneToModify, /*BCS_ParentBoneSpace*/BCS_ParentBoneSpace);
					Output.Pose.SetComponentSpaceTransform(compactPoseBoneToModify, newBoneTM);

					UE_LOG(LogTemp,Display,TEXT("[Anim] %s:%s"),*name.ToString(),*newBoneTM.ToString());
				}
			
			}
		
		}
	}
	else
	{
		FAnimNode_SkeletalControlBase::EvaluateSkeletalControl_AnyThread(Output, OutBoneTransforms);
	}
	


}

bool FAnimNode_BlendFadeIn::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return true;
}


void FAnimNode_BlendFadeIn::GatherDebugData(FNodeDebugData& DebugData)
{
	FAnimNode_Base::GatherDebugData(DebugData);
}

void FAnimNode_BlendFadeIn::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);
}