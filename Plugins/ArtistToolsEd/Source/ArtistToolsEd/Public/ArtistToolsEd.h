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
	
	
private:

	
	//注册设置信息
	void RegisterSettings();
	//卸载设置信息
	void UnregisterSettings();
	//设置信息发生变化
	bool HandleSettingsSaved() const;
	
	void OnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow);
	

private:
	TSharedPtr<class FMainMenuToolbar> MainMenuToolbar;
	TSharedPtr<class FUICommandList> PluginCommands;
	//引擎初始化完毕
	void OnPostEngineInit();
	
};
