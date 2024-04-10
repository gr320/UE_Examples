// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FArtistToolsEdModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();
	//注册设置信息
	void RegisterSettings();
	//卸载设置信息
	void UnregisterSettings();
	//创建下拉菜单
	TSharedRef<SWidget> GenerateComboMenu(TSharedPtr<FUICommandList> InCommands) const;
	void BindCommands();

	void OnToolBarClick();
	//打开设置界面
	void OpenSettings() const;
	//设置信息发生变压
	bool HandleSettingsSaved() const;
	//重启编辑器
	void RestartEditor() const;
	//中英文切换
	void LangSwitcher() const;
	void OpenGitHubUrl() const;

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	
};
