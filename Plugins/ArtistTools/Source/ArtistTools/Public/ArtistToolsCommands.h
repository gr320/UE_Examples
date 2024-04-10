// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ArtistToolsStyle.h"

class FArtistToolsCommands : public TCommands<FArtistToolsCommands>
{
public:

	FArtistToolsCommands()
		: TCommands<FArtistToolsCommands>(TEXT("ArtistTools"), NSLOCTEXT("Contexts", "ArtistTools", "ArtistTools Plugin"), NAME_None, FArtistToolsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > Settings;
	TSharedPtr< FUICommandInfo > Restart;
	TSharedPtr< FUICommandInfo > SwitchLanguage;
	TSharedPtr< FUICommandInfo > Github;
};
