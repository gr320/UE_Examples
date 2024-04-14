// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtistToolsEdStyle.h"
#include "ArtistToolsEd.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FArtistToolsEdStyle::StyleInstance = nullptr;

void FArtistToolsEdStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FArtistToolsEdStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FArtistToolsEdStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ArtistToolsEdStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon30x30(30.0f, 30.0f);
const FVector2D Icon48x48(48.0f, 48.0f);

TSharedRef< FSlateStyleSet > FArtistToolsEdStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ArtistToolsEdStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ArtistToolsEd")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ArtistToolsEd.ArtistToolsLogo", new IMAGE_BRUSH_SVG(TEXT("Icons/Logo_Blue"), Icon20x20));
	Style->Set("ArtistToolsEd.OpenSettings", new IMAGE_BRUSH_SVG(TEXT("Icons/Settings"), Icon20x20));
	Style->Set("ArtistToolsEd.SwitchLanguage", new IMAGE_BRUSH_SVG(TEXT("Icons/SwitchLanguage"), Icon20x20));
	Style->Set("ArtistToolsEd.Language", new IMAGE_BRUSH_SVG(TEXT("Icons/SwitchLanguage"), Icon20x20));
	Style->Set("ArtistToolsEd.LanguageCN", new IMAGE_BRUSH_SVG(TEXT("Icons/flag_ch"), Icon20x20));
	Style->Set("ArtistToolsEd.LanguageEN", new IMAGE_BRUSH_SVG(TEXT("Icons/flag_us"), Icon20x20));
	Style->Set("ArtistToolsEd.Github", new IMAGE_BRUSH_SVG(TEXT("Icons/Github"), Icon20x20));
	Style->Set("ArtistToolsEd.RestartEditor", new IMAGE_BRUSH_SVG(TEXT("Icons/Restart"), Icon20x20));
	Style->Set("ArtistToolsEd.About", new IMAGE_BRUSH_SVG(TEXT("Icons/About"), Icon20x20));
	Style->Set("ArtistToolsEd.Bbackground", new IMAGE_BRUSH_SVG(TEXT("Sprites/Bbackground"), Icon20x20));
	Style->Set("ArtistToolsEd.File", new IMAGE_BRUSH_SVG(TEXT("Icons/file"), Icon30x30));
	
	return Style;
}

void FArtistToolsEdStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FArtistToolsEdStyle::Get()
{
	return *StyleInstance;
}
