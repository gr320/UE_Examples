// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimGraphNode_ApplyBonePose.h"

FText UAnimGraphNode_ApplyBonePose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Apply Bones Node");
}

FText UAnimGraphNode_ApplyBonePose::GetTooltipText() const
{
	return FText::FromString("ApplyBonePoseTooltip");
}


void UAnimGraphNode_ApplyBonePose::Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* PreviewSkelMeshComp) const
{
	DrawAngularLimits(PDI, PreviewSkelMeshComp, Node);
}

void UAnimGraphNode_ApplyBonePose::DrawAngularLimits(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* SkelMeshComp, const FAnimNode_BlendFadeIn& NodeToVisualize) const
{
	
}

FText UAnimGraphNode_ApplyBonePose::GetControllerDescription() const
{
	return Super::GetControllerDescription();
}