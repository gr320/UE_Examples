// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtistToolsStyle.h"
#include "ArtistTools.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FArtistToolsStyle::StyleInstance = nullptr;

void FArtistToolsStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FArtistToolsStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FArtistToolsStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ArtistToolsStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FArtistToolsStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ArtistToolsStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ArtistTools")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ArtistTools.Settings", new IMAGE_BRUSH_SVG(TEXT("Settings"), Icon20x20));
	Style->Set("ArtistTools.SwitchLanguage", new IMAGE_BRUSH_SVG(TEXT("SwitchLanguage"), Icon20x20));
	Style->Set("ArtistTools.Github", new IMAGE_BRUSH_SVG(TEXT("Github"), Icon20x20));
	Style->Set("ArtistTools.Rrefresh", new IMAGE_BRUSH_SVG(TEXT("Rrefresh"), Icon20x20));
	Style->Set("ArtistTools.Restart", new IMAGE_BRUSH_SVG(TEXT("Github"), Icon20x20));
	
	return Style;
}

void FArtistToolsStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FArtistToolsStyle::Get()
{
	return *StyleInstance;
}
