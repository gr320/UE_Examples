#include "ArtistToolsSettings.h"

#include "FInputProcessor.h"

UArtistToolsSettings::UArtistToolsSettings()
{
	LanguageShortcutKey = FInputChord(EKeys::L,false,false,true,false);
	LanguageMap.Add(Chinese, "zh-Hans");
	LanguageMap.Add(English, "en");
	
	// LanguageMap.Add(Arabic, "ar");
	// LanguageMap.Add(French, "fr");
	// LanguageMap.Add(German, "de");
	// LanguageMap.Add(Italian, "it");
	// LanguageMap.Add(Japanese, "ja");
	// LanguageMap.Add(Polish, "pl");
	// LanguageMap.Add(Portuguese, "pt-BR");
	// LanguageMap.Add(Russian, "ru");
	// LanguageMap.Add(Spanish, "es");
	// LanguageMap.Add(Korean, "ko");

	inputp = MakeShareable(new FInputProcessor(this));
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().RegisterInputPreProcessor(inputp);
	}
}

UArtistToolsSettings::~UArtistToolsSettings()
{
	
}

//获取蓝图
void UArtistToolsSettings::GetOpenTheBlueprint()
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();
}
//刷新蓝图
void UArtistToolsSettings::RefreshBlueprints()
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();
	if (EditedAssets.Num() > 0)
	{
		for (UObject* Asset : EditedAssets)
		{
			TWeakObjectPtr<UBlueprint> Blueprint = Cast<UBlueprint>(Asset);
			FBlueprintEditorUtils::RefreshAllNodes(Blueprint.Get());
		}
	}
}
