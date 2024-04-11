// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtistToolsEdCommands.h"

#define LOCTEXT_NAMESPACE "FArtistToolsEdModule"

void FArtistToolsEdCommands::RegisterCommands()
{

	
	UI_COMMAND(ArtistToolsEditor, "ArtistToolsEditor", "ArtistTools Editor", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(OpenSettings, "Settings", "Open ArtistTools Seettings", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(RestartEditor, "RestartEditor", "Restart The Editor", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SwitchLanguage, "SwitchLanguage", "chinese-english switch", EUserInterfaceActionType::CollapsedButton, FInputChord(EKeys::L,EModifierKey::Alt));
	UI_COMMAND(Github, "Github", "Open Github Site", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(About, "About", "Open About Site", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(LanguageCN, "zh-CN", "Chinese", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(LanguageEN, "en-US", "English", EUserInterfaceActionType::RadioButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE
