// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtistToolsCommands.h"

#define LOCTEXT_NAMESPACE "FArtistToolsModule"

void FArtistToolsCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ArtistTools", "Execute ArtistTools action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
