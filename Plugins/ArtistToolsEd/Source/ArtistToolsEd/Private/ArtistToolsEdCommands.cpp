// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtistToolsEdCommands.h"

#define LOCTEXT_NAMESPACE "FArtistToolsEdModule"

void FArtistToolsEdCommands::RegisterCommands()
{

	
	UI_COMMAND(ArtistToolsEditor, "ArtistToolsEditor", "ArtistTools Editor", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Settings, "Settings", "Open ArtistTools Seettings", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(RestartEditor, "RestartEditor", "Restart The Editor", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SwitchLanguage, "SwitchLanguage", "chinese-english switch", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Github, "Github", "Open Github Site", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
