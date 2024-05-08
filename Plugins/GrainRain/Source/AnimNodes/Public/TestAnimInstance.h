// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/BoneTransforms.h"
#include "TestAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ANIMNODES_API UTestAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category=HeartDub)
	FCustomBoneTransforms TestBoneTransforms;
};
