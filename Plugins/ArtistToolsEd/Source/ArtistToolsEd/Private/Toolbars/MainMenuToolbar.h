#pragma once

#include "CoreMinimal.h"

class FMainMenuToolbar
{
public:
	FMainMenuToolbar();

	FMainMenuToolbar(TSharedRef<FUICommandList> InCommandList);

	virtual ~FMainMenuToolbar() = default;

	static const FName ArtistToolsEdTabName;

	//初始化
	void Initialize();

	//绑定指令
	void BindCommands();
	
	//切花语言。
	static void LangSwitcher();
	
private:
	void OnToolBarClick();

	void OpenLauncher();

	void RegisterTabSpawners();

	void UnregisterTabSpawners();
	
	//构建下拉菜单.
	TSharedRef<SWidget> GenerateComboMenu() const;
	
	const TSharedRef<FUICommandList> CommandList;

	TSharedPtr<class FPackageTools> PackageTools;
	
};
