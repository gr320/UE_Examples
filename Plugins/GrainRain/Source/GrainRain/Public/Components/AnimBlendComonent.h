// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimBlendComonent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRAINRAIN_API UAnimBlendComonent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAnimBlendComonent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable,CallInEditor)
	void Init(USkeletalMeshComponent* InSkeletalMeshComponent);
	UFUNCTION(BlueprintCallable,CallInEditor)
	void CopyPose(FBoneTransforms& OutBoneTransforms);

public:
	USkeletalMeshComponent* SkeletalMeshComponent;
};
