// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ArtistToolsEdStyle.h"

class FArtistToolsEdCommands : public TCommands<FArtistToolsEdCommands>
{
public:

	FArtistToolsEdCommands()
		: TCommands<FArtistToolsEdCommands>(TEXT("ArtistToolsEd"), NSLOCTEXT("Contexts", "ArtistToolsEd", "ArtistToolsEd Plugin"), NAME_None, FArtistToolsEdStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:

	TSharedPtr< FUICommandInfo > ArtistToolsEditor;
	TSharedPtr< FUICommandInfo > OpenSettings;
	TSharedPtr< FUICommandInfo > RestartEditor;
	TSharedPtr< FUICommandInfo > SwitchLanguage;
	TSharedPtr< FUICommandInfo > Github;
	TSharedPtr< FUICommandInfo > About;

	TSharedPtr< FUICommandInfo > LanguageCN;
	TSharedPtr< FUICommandInfo > LanguageEN;

	TSharedPtr< FUICommandInfo > Launcher;
	TSharedPtr< FUICommandInfo > PackageTools;
};
