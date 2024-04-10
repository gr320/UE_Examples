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
#include "Internationalization/Culture.h"
#include "Kismet/KismetInternationalizationLibrary.h"

static const FName ArtistToolsEdTabName("ArtistToolsEd");

#define LOCTEXT_NAMESPACE "FArtistToolsEdModule"

void FArtistToolsEdModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FArtistToolsEdStyle::Initialize();
	FArtistToolsEdStyle::ReloadTextures();

	FArtistToolsEdCommands::Register();
	BindCommands();
	RegisterSettings();
	RegisterMenus();
	PluginCommands = MakeShareable(new FUICommandList);

	
}

void FArtistToolsEdModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UnregisterSettings();
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FArtistToolsEdStyle::Shutdown();

	FArtistToolsEdCommands::Unregister();
}

void FArtistToolsEdModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	auto OnGetToolbarButtonBrushLambda = [this]() -> const FSlateIcon
	{
		//可以根据状态获取Icon
		return FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(),"ArtistTools.ArtistToolsLogo");
	};
	
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FArtistToolsEdCommands::Get().ArtistToolsEditor, PluginCommands);
		}
	}

	{
		//工具栏中添加按键.
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("ArtistToolsEditor");
			{

				Section.AddDynamicEntry(ArtistToolsEdTabName, FNewToolMenuSectionDelegate::CreateLambda([&](FToolMenuSection& InDynamicSection)
				{
					InDynamicSection.AddEntry(FToolMenuEntry::InitToolBarButton(
								ArtistToolsEdTabName,
								FUIAction(FExecuteAction::CreateRaw(this,&FArtistToolsEdModule::OnToolBarClick)),
								LOCTEXT("ArtistToolsEditor", "ArtistTools Editor"),
								LOCTEXT("ArtistToolsEditorTip", "ArtistTools Editor Tip."),
								TAttribute<FSlateIcon>::Create(OnGetToolbarButtonBrushLambda)
					));
					//构建下拉框.
					InDynamicSection.AddEntry(FToolMenuEntry::InitComboButton(
						"ADTools ComboMenu",
						FUIAction(),
						FOnGetContent::CreateRaw(this,&FArtistToolsEdModule::GenerateComboMenu, PluginCommands),
						LOCTEXT("LaunchCombo_Label", "ADTools Options"),
						LOCTEXT("ComboToolTip", "Options for ADTools"),
						FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(), "ArtistTools.ArtistToolsLogo"),
						true
					));
					
				}));
			}
		}
	}
}

//创建下拉菜单
TSharedRef<SWidget> FArtistToolsEdModule::GenerateComboMenu(TSharedPtr<FUICommandList> InCommands) const
{
	//注册UWEditorButton
	//UToolMenus::Get()->RegisterMenu("LevelEditor.LevelEditorToolBar.ArtistToolsEdButton");

	//构建 MenuBuilder
	FMenuBuilder MenuBuilder(true, InCommands);

	MenuBuilder.BeginSection("ArtistToolsMenu", TAttribute<FText>(FText::FromString("ArtistToolsMenu")));

	
	MenuBuilder.AddMenuEntry(FArtistToolsEdCommands::Get().Settings);
	MenuBuilder.AddMenuEntry(FArtistToolsEdCommands::Get().SwitchLanguage);
	MenuBuilder.AddMenuSeparator();
	MenuBuilder.AddMenuEntry(FArtistToolsEdCommands::Get().RestartEditor);
	MenuBuilder.AddMenuSeparator();
	MenuBuilder.AddMenuEntry(FArtistToolsEdCommands::Get().Github);
	MenuBuilder.EndSection();
	
	return MenuBuilder.MakeWidget();
}
void FArtistToolsEdModule::BindCommands()
{
	check(!PluginCommands.IsValid());

	const FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	PluginCommands = LevelEditorModule.GetGlobalLevelEditorActions();
	
	const FArtistToolsEdCommands& Commands = FArtistToolsEdCommands::Get();
	FUICommandList& ActionList = *PluginCommands;


	ActionList.MapAction(
				FArtistToolsEdCommands::Get().Settings,
				FExecuteAction::CreateRaw(this, &FArtistToolsEdModule::OpenSettings),
				FCanExecuteAction());

	ActionList.MapAction(
			FArtistToolsEdCommands::Get().ArtistToolsEditor,
			FExecuteAction::CreateRaw(this, &FArtistToolsEdModule::OnToolBarClick),
			FCanExecuteAction());
	ActionList.MapAction(
		Commands.SwitchLanguage,
		FExecuteAction::CreateRaw(this,&FArtistToolsEdModule::LangSwitcher),
		FCanExecuteAction());

	//重启按键
	ActionList.MapAction(
		Commands.RestartEditor,
		FExecuteAction::CreateRaw(this, &FArtistToolsEdModule::RestartEditor),
		FCanExecuteAction());
	//打开链接地址
	ActionList.MapAction(
		Commands.Github,
		FExecuteAction::CreateRaw(this, &FArtistToolsEdModule::OpenGitHubUrl),
		FCanExecuteAction());

	
	//AddPIEPreviewDeviceActions(Commands, ActionList);
}
//注册设置信息
void FArtistToolsEdModule::RegisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		const ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", ArtistToolsEdTabName,
			LOCTEXT("AdToolsSetting", "ADTools"),
			LOCTEXT("AdToolsSettingDescription", "Configure the ADTools plug-in."),
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
		SettingsModule->UnregisterSettings("Project", "Plugins", ArtistToolsEdTabName);

	}
}

void FArtistToolsEdModule::OnToolBarClick()
{
	UE_LOG(LogTemp,Warning,TEXT("OnToolBarClick"))
}

//打开设置界面
void FArtistToolsEdModule::OpenSettings() const
{

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->ShowViewer("Project", "Plugins", ArtistToolsEdTabName);
	}

}
//设置信息发生变压
bool FArtistToolsEdModule::HandleSettingsSaved() const
{
	return true;
}
//重启编辑器
void FArtistToolsEdModule::RestartEditor() const
{
	//重启编辑器
	FUnrealEdMisc::Get().RestartEditor((true));
}
//中英文切换
void FArtistToolsEdModule::LangSwitcher() const
{
	//中英文切换
	if(UKismetInternationalizationLibrary::GetCurrentLanguage() == "zh-hans")
	{
		UKismetInternationalizationLibrary::SetCurrentLanguage("en",true);
	}
	else
	{
		UKismetInternationalizationLibrary::SetCurrentLanguage("zh-hans",true);
	}
}
void FArtistToolsEdModule::OpenGitHubUrl() const
{
	FPlatformProcess::LaunchURL(TEXT("https://github.com/wzxdev/ADTools"), nullptr, nullptr);
}
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArtistToolsEdModule, ArtistToolsEd)