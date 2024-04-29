﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AnimBlendComonent.h"

#include "Animation/AnimSingleNodeInstance.h"
#include "Data/BoneTransforms.h"


// Sets default values for this component's properties
UAnimBlendComonent::UAnimBlendComonent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAnimBlendComonent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAnimBlendComonent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAnimBlendComonent::Init(USkeletalMeshComponent* InSkeletalMeshComponent)
{
    SkeletalMeshComponent  = InSkeletalMeshComponent;
	
}

void UAnimBlendComonent::CopyPose(FBoneTransforms& OutBoneTransforms)
{
	TArray<FName> BoneNames;
	SkeletalMeshComponent->GetBoneNames(BoneNames);


	UAnimSingleNodeInstance* SingleNodeInstance = SkeletalMeshComponent->GetSingleNodeInstance();

	if (SingleNodeInstance == nullptr)
	{
		return;
	}
	
	FBoneContainer BoneContainer = SingleNodeInstance->GetRequiredBones();
	
	BoneContainer.SetUseRAWData(true);

	FCompactPose Pose;
	Pose.SetBoneContainer(&BoneContainer);
	Pose.ResetToRefPose();
	
	FBlendedCurve TempCurve;
	TempCurve.InitFrom(BoneContainer);
	UE::Anim::FStackAttributeContainer TempAttributes;

	FAnimationPoseData AnimPoseData(Pose, TempCurve, TempAttributes);

	UAnimSequence* AnimSequence = Cast<UAnimSequence>(SingleNodeInstance->GetCurrentAsset());
	double Time = 0;
	FDeltaTimeRecord DeltaTimeRecord;
	DeltaTimeRecord.Set(0, 0.f);
	FAnimExtractContext ExtractionContext(Time, false, DeltaTimeRecord, true);
	
	AnimSequence->GetBonePose(AnimPoseData,ExtractionContext,true);


	
	
	for (int i = 0; i < BoneNames.Num(); ++i)
	{
		int32 boneIndex = SkeletalMeshComponent->GetSkeletalMeshAsset()->GetRefSkeleton().FindBoneIndex(BoneNames[i]);
		FName boneName = SkeletalMeshComponent->GetSkeletalMeshAsset()->GetRefSkeleton().GetBoneName(boneIndex);
		//BoneContainer.GetPoseBoneIndexForBoneName(boneName);
		FCompactPoseBoneIndex CompactIndex(boneIndex);

		//Pose.GetRefPose(CompactIndex)
		FTransform transform = Pose.GetRefPose(CompactIndex);
		
		OutBoneTransforms.Transforms.Add(transform);
		OutBoneTransforms.Names.Add(boneName.ToString());
		UE_LOG(LogTemp,Display,TEXT("%s"),*transform.GetLocation().ToString())
	}

	//OutBoneTransforms.Transforms =AnimPoseData.GetPose().GetBones();
	
}
