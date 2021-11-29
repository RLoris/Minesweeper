// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class SEditableTextBox;
class SUniformGridPanel;

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
	
private:

	void Debug(FString Text);

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
