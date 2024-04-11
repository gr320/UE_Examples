// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtistToolsEd.h"
#include "ArtistToolsEdStyle.h"
#include "ArtistToolsEdCommands.h"
#include "ArtistToolsSettings.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "LevelEditor.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Interfaces/IMainFrameModule.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "Kismet2/DebuggerCommands.h"
#include "Toolbars/MainMenuToolbar.h"


#define LOCTEXT_NAMESPACE "FArtistToolsEdModule"

void FArtistToolsEdModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FArtistToolsEdStyle::Initialize();
	FArtistToolsEdStyle::ReloadTextures();
	FArtistToolsEdCommands::Register();

	
	/*
	// LevelEditor 获取的 CommandList 快捷键,在视口内不可用.
	const FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedRef<FUICommandList> CommandList = LevelEditorModule.GetGlobalLevelEditorActions();
	MainMenuToolbar = MakeShareable(new FMainMenuToolbar(CommandList));
	*/
	
	MainMenuToolbar = MakeShareable(new FMainMenuToolbar);
	
	
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FArtistToolsEdModule::OnPostEngineInit);

}
//引擎初始化完毕
void FArtistToolsEdModule::OnPostEngineInit()
{
	if (!GEditor)
	{
		// Loading MainFrame module with '-game' is not supported
		return;
	}
	RegisterSettings();
	
	MainMenuToolbar->Initialize();

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameModule.OnMainFrameCreationFinished().AddRaw(this, &FArtistToolsEdModule::OnMainFrameCreationFinished);
}

void FArtistToolsEdModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UnregisterSettings();

	if (GIsEditor)
	{
		FArtistToolsEdStyle::Shutdown();
	}
	
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FArtistToolsEdCommands::Unregister();
}

//注册设置信息
void FArtistToolsEdModule::RegisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		const ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", FMainMenuToolbar::ArtistToolsEdTabName,
			LOCTEXT("ArtistTools", "Artist Tools"),
			LOCTEXT("ArtistToolsDescription", "Configure the Artist Tools plug-in."),
			GetMutableDefault<UArtistToolsSettings>()
		);
		
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FArtistToolsEdModule::HandleSettingsSaved);
		}

	}
}

//卸载设置信息
void FArtistToolsEdModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", FMainMenuToolbar::ArtistToolsEdTabName);

	}
}
//设置信息发生变化
bool FArtistToolsEdModule::HandleSettingsSaved() const
{
	return true;
}

void FArtistToolsEdModule::OnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
{
	//注册切换语言快捷键....
	FPlayWorldCommands::GlobalPlayWorldActions->MapAction(
		FArtistToolsEdCommands::Get().SwitchLanguage, FExecuteAction::CreateStatic(FMainMenuToolbar::LangSwitcher), FCanExecuteAction()
   );
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArtistToolsEdModule, ArtistToolsEd)