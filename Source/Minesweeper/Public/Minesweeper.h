// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class SEditableTextBox;
class SUniformGridPanel;
class SButton;

class FMinesweeperModule : public IModuleInterface
{
public:
	/* Widgets */
	TSharedPtr<SEditableTextBox> HeightBox;
	TSharedPtr<SEditableTextBox> WidthBox;
	TSharedPtr<SEditableTextBox> BombBox;
	TSharedPtr<SUniformGridPanel> MineGrid;
	TSharedPtr<STextBlock> InfoBlock;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	FReply GenerateGrid();
	void UnveilTile(int32 curCol, int32 curRow, int32 RowCount, int32 ColCount, int32 BombCount);
	
private:

	void Debug(FString Text);
	void InitializeGame(int32 Height, int32 Width, int32 BombCount, int32 InitIdx);
	TArray<bool> BombLocations;
	bool IsGameInitialized = false;
	int32 TilesLeft = 0;
	bool IsGameOver = false;

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
