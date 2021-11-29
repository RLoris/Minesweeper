// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperModule"

void FMinesweeperCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Minesweeper", "Play Minesweeper now !", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
