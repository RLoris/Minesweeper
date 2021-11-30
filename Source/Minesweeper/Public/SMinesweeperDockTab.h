// RLoris 2021

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"

class SEditableTextBox;
class SUniformGridPanel;

class SMinesweeperDockTab : public SDockTab
{
public:
	/* Widgets */
	TSharedPtr<SEditableTextBox> HeightBox;
	TSharedPtr<SEditableTextBox> WidthBox;
	TSharedPtr<SEditableTextBox> BombBox;
	TSharedPtr<SUniformGridPanel> MineGrid;
	TSharedPtr<STextBlock> InfoBlock;
	TArray<bool> BombLocations;
	bool bIsGameInitialized = false;
	int32 TilesLeft = 0;
	bool bIsGameOver = false;
public:
	void Construct(const FArguments& InArgs);
	FReply GenerateGrid();
	void Debug(FString Text);
	void InitializeGame(int32 Height, int32 Width, int32 BombCount, int32 InitIdx);
	void UnveilTile(int32 CurCol, int32 CurRow, int32 RowCount, int32 ColCount, int32 BombCount);
};