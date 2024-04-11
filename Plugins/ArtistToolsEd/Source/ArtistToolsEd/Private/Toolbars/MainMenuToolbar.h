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
	
	
	static void LangSwitcher();
	
private:
	void OnToolBarClick();
	
	//构建下拉菜单.
	TSharedRef<SWidget> GenerateComboMenu() const;
	
	const TSharedRef<FUICommandList> CommandList;
	
};
