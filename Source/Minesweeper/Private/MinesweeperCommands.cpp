// RLoris 2021

#include "MinesweeperCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperModule"

void FMinesweeperCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Minesweeper", "Play Minesweeper now !", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
