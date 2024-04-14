// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/Object.h"
#include "ArtistToolsSettings.generated.h"


UENUM()
enum ELanguage
{
	Chinese,
	English,
	
	/*
	Arabic,
	French,
	German,
	Italian,
	Japanese,
	Polish,
	Portuguese,
	Russian,
	Spanish,
	Korean,
	*/
};

/**
 * 设置信息
 */
UCLASS(Config = ArtistTools)
class ARTISTTOOLSED_API UArtistToolsSettings : public UObject
{
	GENERATED_BODY()
public:

	UArtistToolsSettings();
	~UArtistToolsSettings();
	//设置。。。。

public:


	

	//获取已经打开的
	void GetOpenTheBlueprint();
	//刷新蓝图。。。
	void RefreshBlueprints();


	TArray<UObject*>EditedAssets;
	
	TSharedPtr<class FInputProcessor> inputp;

	FString  Switch;
	FString Previous;
	
	UPROPERTY(Config,EditAnywhere,Category=ArtistToolsEditor)
	FString TestStringValue;
	UPROPERTY(Config,EditAnywhere,Category=ArtistToolsEditor)
	FInputChord LanguageShortcutKey;
	UPROPERTY(Config, EditAnywhere, Category = ArtistToolsEditor)
	TEnumAsByte<ELanguage> Language = ELanguage::Chinese;
	TMap<ELanguage, FString>LanguageMap;
};
