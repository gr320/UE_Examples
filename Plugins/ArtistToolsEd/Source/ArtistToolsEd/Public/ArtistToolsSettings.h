// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ArtistToolsSettings.generated.h"

/**
 * 设置信息
 */
UCLASS(Config = ArtistTools)
class ARTISTTOOLSED_API UArtistToolsSettings : public UObject
{
	GENERATED_BODY()
public:

	//设置。。。。
	UPROPERTY(EditAnywhere,Category=ArtistToolsEditor)
	FString TestStringValue;
};
