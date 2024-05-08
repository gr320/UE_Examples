// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/AnimGraph/Public/AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_BlendFadeIn.h"
#include "AnimGraphNode_ApplyBonePose.generated.h"

/**
 * 
 */
UCLASS(meta = (Keywords = "BlendFadeIn"))
class ARTISTTOOLSED_API UAnimGraphNode_ApplyBonePose : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = HeartDub)
	FAnimNode_BlendFadeIn Node; // 
	
	
public:

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

protected:
	
	void DrawAngularLimits(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* SkelMeshComp, const FAnimNode_BlendFadeIn& NodeToVisualize) const;

	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	virtual void Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent * PreviewSkelMeshComp) const override;
};
