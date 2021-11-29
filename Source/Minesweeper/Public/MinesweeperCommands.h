// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MinesweeperStyle.h"

class FMinesweeperCommands : public TCommands<FMinesweeperCommands>
{
public:

	FMinesweeperCommands()
		: TCommands<FMinesweeperCommands>(TEXT("Minesweeper"), NSLOCTEXT("Contexts", "Minesweeper", "Minesweeper Plugin"), NAME_None, FMinesweeperStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};