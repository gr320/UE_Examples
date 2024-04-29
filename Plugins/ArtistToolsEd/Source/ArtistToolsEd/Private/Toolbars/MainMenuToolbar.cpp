#include "MainMenuToolbar.h"

#include "ArtistToolsEdCommands.h"
#include "ISettingsModule.h"
#include "PackageTools.h"
#include "Interfaces/IMainFrameModule.h"
#include "ToolMenus.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "SlateUI/About/AboutWidget.h"
#include "SlateUI/Lanucher/ArtistToolsLauncherWindow.h"


#define LOCTEXT_NAMESPACE "ArtistToolsMainMenuToolbar"
const FName FMainMenuToolbar::ArtistToolsEdTabName(TEXT("ArtistToolsEd"));


FMainMenuToolbar::FMainMenuToolbar()
    : CommandList(new FUICommandList)
{
	BindCommands();
}

FMainMenuToolbar::FMainMenuToolbar(TSharedRef<FUICommandList> InCommandList):CommandList(InCommandList)
{
	BindCommands();
}
//初始化
void FMainMenuToolbar::Initialize()
{
	
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);


	PackageTools = MakeShared<FPackageTools>();
	
	RegisterTabSpawners();

	
	{
    	//Window 下拉列表中添加.
    	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	    {
    		FToolMenuSection& Section = Menu->FindOrAddSection("ArtistTools");
    		//CommandList MapAction
    		Section.AddMenuEntryWithCommandList(FArtistToolsEdCommands::Get().ArtistToolsEditor, CommandList,TAttribute<FText>(),TAttribute<FText>(),
    			FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(), "ArtistToolsEd.ArtistToolsLogo"));
	    }
	}


	//根据需求,返回不同Icon.适合在ToolBar上做状态区别.
	auto OnGetToolbarButtonBrushLambda = [this]() -> const FSlateIcon
	{
		//可以根据状态获取Icon
		return FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(),"ArtistToolsEd.ArtistToolsLogo");
	};
	
	

	{
		//工具栏中添加按键.
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("ArtistToolsEditor");
			{
				Section.AddDynamicEntry(ArtistToolsEdTabName, FNewToolMenuSectionDelegate::CreateLambda([&](FToolMenuSection& InDynamicSection)
				{
					//添加主菜单按键.
					FToolMenuEntry MainButton1 = FToolMenuEntry::InitToolBarButton(
								ArtistToolsEdTabName,
								FUIAction(FExecuteAction::CreateRaw(this,&FMainMenuToolbar::OnToolBarClick)),
								LOCTEXT("ArtistToolsMainButton", "ArtistTools MainButton"),
								LOCTEXT("ArtistToolsMainButtonTip", "ArtistTools MainButton Tip"),
								TAttribute<FSlateIcon>::Create(OnGetToolbarButtonBrushLambda)
								);
					//显示文本
					//MainButton1.StyleNameOverride = "CalloutToolbar";
					InDynamicSection.AddEntry(MainButton1);

					//构建下拉框
					FToolMenuEntry ComboButton1 = FToolMenuEntry::InitComboButton(
						"ADToolsMenu1",
						FUIAction(),
						FOnGetContent::CreateRaw(this,&FMainMenuToolbar::GenerateComboMenu),
						LOCTEXT("ArtistToolsOptions1", "Artist Tools Options 1"),
						LOCTEXT("ArtistToolsOptionsTip1", "Artist Tools Options Tip 1"),
						FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(), "ArtistToolsEd.ArtistToolsLogo"),
						true
					);
					//bInSimpleComboBox [true:简单样式,三个点竖着排列] [false:标准样式,图标加向下箭头.v]
					//向下箭头前显示文本
					//ComboButton1.StyleNameOverride = "CalloutToolbar";
					InDynamicSection.AddEntry(ComboButton1);
					
				}));

				/*
				 //其他样式....
				 FToolMenuEntry ComboButton2 = FToolMenuEntry::InitComboButton(
							"ADToolsMenu2",
							FUIAction(FExecuteAction::CreateRaw(this,&FMainMenuToolbar::OnToolBarClick)),
							FOnGetContent::CreateRaw(this,&FMainMenuToolbar::GenerateComboMenu),
							LOCTEXT("ArtistToolsOptions2", "Artist Tools Options 2"),
							LOCTEXT("ArtistToolsOptionsTip2", "ArtistTools Options Tip 2"),
							TAttribute<FSlateIcon>::CreateLambda([]()
							{
								//可以根据状态获取Icon
								return FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(),"ArtistToolsEd.ArtistToolsLogo");
							}));
				//向下箭头前显示文本
				//ComboButton2.StyleNameOverride = "CalloutToolbar";
				Section.AddEntry(ComboButton2);


				FToolMenuEntry ComboButton3 = FToolMenuEntry::InitComboButton(
				"ADToolsMenu3",
				FUIAction(),
				FOnGetContent::CreateRaw(this,&FMainMenuToolbar::GenerateComboMenu),
				LOCTEXT("ArtistToolsOptions3", "ATO"),
				LOCTEXT("ArtistToolsOptionsTip3", "ArtistTools Options Tip 3"),
				TAttribute<FSlateIcon>::Create(OnGetToolbarButtonBrushLambda));

				//向下箭头前显示文本
				ComboButton3.StyleNameOverride = "CalloutToolbar";
				Section.AddEntry(ComboButton3);*/
			}
		}
	}
    
}
//绑定指令
void FMainMenuToolbar::BindCommands()
{

	
	//Launcher
	CommandList->MapAction(FArtistToolsEdCommands::Get().Launcher, FExecuteAction::CreateRaw(this,&FMainMenuToolbar::OpenLauncher), FCanExecuteAction());

	//重启编辑器
	CommandList->MapAction(FArtistToolsEdCommands::Get().RestartEditor, FExecuteAction::CreateLambda([]
    {
		//重启编辑器
		FUnrealEdMisc::Get().RestartEditor(true);
    }), FCanExecuteAction());

	//切换语言.
    CommandList->MapAction(FArtistToolsEdCommands::Get().SwitchLanguage, FExecuteAction::CreateStatic(FMainMenuToolbar::LangSwitcher), FCanExecuteAction());
	
	//打包Windows
	CommandList->MapAction(FArtistToolsEdCommands::Get().PackageTools, FExecuteAction::CreateLambda([this]
	{
		//重启编辑器
		PackageTools->OnPackageProject();
	}), FCanExecuteAction());
	

	
	CommandList->MapAction(FArtistToolsEdCommands::Get().LanguageEN,
		FUIAction(FExecuteAction::CreateLambda([]()
		{
			if (UKismetInternationalizationLibrary::GetCurrentLanguage() !="en")
			{
				UKismetInternationalizationLibrary::SetCurrentLanguage("en",true);
			}
			
		}),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([]()
		{
			return UKismetInternationalizationLibrary::GetCurrentLanguage() =="en";
		})
	));
	CommandList->MapAction(FArtistToolsEdCommands::Get().LanguageCN, FUIAction(FExecuteAction::CreateLambda([]()
		{
			if (UKismetInternationalizationLibrary::GetCurrentLanguage() !="zh-Hans")
			{
				UKismetInternationalizationLibrary::SetCurrentLanguage("zh-Hans",true);
			}
			
		}),
		FCanExecuteAction(),
		FIsActionChecked::CreateLambda([]()
		{
			
			return UKismetInternationalizationLibrary::GetCurrentLanguage() =="zh-Hans";
		})
	));

	//打开设置
	CommandList->MapAction(FArtistToolsEdCommands::Get().OpenSettings, FExecuteAction::CreateLambda([]
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			// SettingsModule->ShowViewer("Project", "Project", "Maps");
			SettingsModule->ShowViewer("Project", "Plugins", "ArtistToolsEd");
		}
	}), FCanExecuteAction());

	//打开指定URL
    CommandList->MapAction(FArtistToolsEdCommands::Get().Github, FExecuteAction::CreateLambda([]
    {
        //FPlatformProcess::LaunchURL(TEXT("https://github.com/gr320/UE_Examples"), nullptr, nullptr);
        const TCHAR* URL = TEXT("cmd");
        const TCHAR* Params = TEXT("/k start https://github.com/gr320/UE_Examples");
        FPlatformProcess::ExecProcess(URL, Params, nullptr, nullptr, nullptr);
    }), FCanExecuteAction());

	//MainToolBar Button点击事件
	CommandList->MapAction(FArtistToolsEdCommands::Get().ArtistToolsEditor, FExecuteAction::CreateRaw(this,&FMainMenuToolbar::OnToolBarClick), FCanExecuteAction());
	

	
	//打开About 窗口.
    CommandList->MapAction(FArtistToolsEdCommands::Get().About, FExecuteAction::CreateLambda([]
    {
        const FText AboutWindowTitle = LOCTEXT("AboutArtistTools", "About Artist Tools");

        TSharedPtr<SWindow> AboutWindow =
            SNew(SWindow)
            .Title(AboutWindowTitle)
            .ClientSize(FVector2D(600.f, 200.f))
            .SupportsMaximize(false).SupportsMinimize(false)
            .SizingRule(ESizingRule::FixedSize)
            [
                SNew(SAboutWidget)
            ];

        IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
        TSharedPtr<SWindow> ParentWindow = MainFrame.GetParentWindow();

        if (ParentWindow.IsValid())
            FSlateApplication::Get().AddModalWindow(AboutWindow.ToSharedRef(), ParentWindow.ToSharedRef());
        else
            FSlateApplication::Get().AddWindow(AboutWindow.ToSharedRef());
    }), FCanExecuteAction());
}


//构建下拉菜单.
TSharedRef<SWidget> FMainMenuToolbar::GenerateComboMenu() const
{
    const FArtistToolsEdCommands& Commands = FArtistToolsEdCommands::Get();
    FMenuBuilder MenuBuilder(true, CommandList);

	FName ExtensionHook = NAME_None;
	//Section ArtistTools
	MenuBuilder.BeginSection(ExtensionHook, TAttribute<FText>(FText::FromString("Artist Tools")));
	MenuBuilder.AddMenuEntry(Commands.Launcher);
	MenuBuilder.AddMenuEntry(Commands.RestartEditor);
	MenuBuilder.EndSection();

	
	MenuBuilder.BeginSection(ExtensionHook, TAttribute<FText>(FText::FromString("Language")));
	MenuBuilder.AddMenuEntry(Commands.SwitchLanguage);
	//添加子菜单
	MenuBuilder.AddSubMenu(LOCTEXT("Sectiono_Language","Language"),LOCTEXT("Sectiono_Language","Language"),
		FNewMenuDelegate::CreateLambda([Commands](FMenuBuilder& SubMenuBuilder)
		{
			//中文
			SubMenuBuilder.AddMenuEntry(Commands.LanguageCN);
			//英文
			SubMenuBuilder.AddMenuEntry(Commands.LanguageEN);
		}),false,FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(),"ArtistToolsEd.Language"));
	
	//MenuBuilder.AddMenuSeparator();//添加一条横线

	MenuBuilder.AddMenuEntry(Commands.PackageTools);
	MenuBuilder.EndSection();



	
	//Section Settings
	MenuBuilder.BeginSection(ExtensionHook, TAttribute<FText>(FText::FromString("Settings")));
	MenuBuilder.AddMenuEntry(Commands.OpenSettings);
	MenuBuilder.EndSection();
	
	//Section About
	MenuBuilder.BeginSection(ExtensionHook, TAttribute<FText>(FText::FromString("About")));
	MenuBuilder.AddMenuEntry(Commands.Github);
	MenuBuilder.AddMenuEntry(Commands.About);
	MenuBuilder.EndSection();
	
    return MenuBuilder.MakeWidget();
}


void FMainMenuToolbar::OnToolBarClick()
{
	UE_LOG(LogTemp,Warning,TEXT("OnToolBarClick"))
}

void FMainMenuToolbar::OpenLauncher()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FTabId("Launcher"));
}

void FMainMenuToolbar::RegisterTabSpawners()
{

	FGlobalTabmanager::Get()->RegisterTabSpawner("Launcher", FOnSpawnTab::CreateLambda([](const FSpawnTabArgs& Args)
	{
		return SNew(SDockTab)
		.Label(LOCTEXT("ArtistToolsLauncher", "ArtistTools Launcher"))
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SArtistToolsLauncherWindow)
		];
	}))
	.SetDisplayName(LOCTEXT("CalleesTabTitle", "Callees"))
	.SetMenuType(ETabSpawnerMenuType::Hidden)
	.SetIcon(FSlateIcon(FArtistToolsEdStyle::GetStyleSetName(), "ArtistToolsEd.ArtistToolsLogo"));

}

void FMainMenuToolbar::UnregisterTabSpawners()
{

	FGlobalTabmanager::Get()->UnregisterTabSpawner("Launcher");
}

//中英文切换
void FMainMenuToolbar::LangSwitcher()
{
	//中英文切换
	if(UKismetInternationalizationLibrary::GetCurrentLanguage() == "zh-Hans")
	{
		UKismetInternationalizationLibrary::SetCurrentLanguage("en",true);
	}
	else
	{
		UKismetInternationalizationLibrary::SetCurrentLanguage("zh-Hans",true);
	}
}

#undef LOCTEXT_NAMESPACE
