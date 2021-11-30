// RLoris 2021

#include "SMinesweeperDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Engine/Font.h"

#define LOCTEXT_NAMESPACE "SMinesweeperDockTab"

void SMinesweeperDockTab::Construct(const FArguments& InArgs)
{
	SDockTab::Construct(InArgs);
	TabRole = ETabRole::NomadTab;
	SetHAlign(HAlign_Fill);
	SetVAlign(VAlign_Fill);
	bShouldAutosize = false;
	UFont* Font = LoadObject<UFont>(nullptr, TEXT("/Engine/EngineFonts/Roboto.Roboto"), nullptr, LOAD_None, nullptr);
	SetContent(
		SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().HAlign(HAlign_Center).Padding(5)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("width", "Width"))
				]
				+ SHorizontalBox::Slot()
				[
					SAssignNew(WidthBox, SEditableTextBox)
					.Text(FText::FromString(TEXT("3")))
				]
				+ SHorizontalBox::Slot().HAlign(HAlign_Center).Padding(5)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("height", "Height"))
				]
				+ SHorizontalBox::Slot()
				[
					SAssignNew(HeightBox, SEditableTextBox)
					.Text(FText::FromString(TEXT("3")))
				]
				+ SHorizontalBox::Slot().HAlign(HAlign_Center).Padding(5)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("mine_count", "Mine count"))
				]
				+ SHorizontalBox::Slot()
				[
					SAssignNew(BombBox, SEditableTextBox)
					.Text(FText::FromString(TEXT("3")))
				]
				+ SHorizontalBox::Slot().Padding(5, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.ButtonColorAndOpacity(FSlateColor(FLinearColor::Green))
					.OnClicked_Raw(this, &SMinesweeperDockTab::GenerateGrid)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("generate_grid", "Generate grid"))
					]
				]
			]
			+ SVerticalBox::Slot().HAlign(HAlign_Center).Padding(5).AutoHeight()
			[
				SAssignNew(InfoBlock, STextBlock).Font(FSlateFontInfo((UObject*) Font, 28))
			]
			+ SVerticalBox::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(5).FillHeight(1.0)
			[
				SAssignNew(MineGrid, SUniformGridPanel)
			]
	);
	// generate grid after construction to play
	GenerateGrid();
}

FReply SMinesweeperDockTab::GenerateGrid()
{
	if (WidthBox.IsValid() && HeightBox.IsValid() && BombBox.IsValid() && MineGrid.IsValid() && InfoBlock.IsValid())
	{
		int32 ColCount = FCString::Atoi(*WidthBox->GetText().ToString());
		int32 RowCount = FCString::Atoi(*HeightBox->GetText().ToString());
		int32 BombCount = FCString::Atoi(*BombBox->GetText().ToString());
		if (ColCount <= 0)
		{
			InfoBlock->SetText(LOCTEXT("invalid_width", "The width is invalid !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else if (RowCount <= 0)
		{
			InfoBlock->SetText(LOCTEXT("invalid_height", "The height is invalid !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else if (BombCount <= 0)
		{
			InfoBlock->SetText(LOCTEXT("invalid_mine", "The mine count is invalid !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else if (ColCount > 16 || RowCount > 16)
		{
			InfoBlock->SetText(LOCTEXT("invalid_size", "The grid width or height is greater than 16 !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
		else if (BombCount > (RowCount * ColCount - 1))
		{
			InfoBlock->SetText(LOCTEXT("invalid_count", "The mine count is greater than the size of the grid !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
		else
		{
			InfoBlock->SetText(LOCTEXT("start_game", "Click on a tile to start"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			MineGrid->ClearChildren();
			TilesLeft = ColCount * RowCount;
			bIsGameInitialized = false;
			bIsGameOver = false;
			for (int32 CurRow = 0; CurRow < RowCount; CurRow++)
			{
				for (int32 CurCol = 0; CurCol < ColCount; CurCol++)
				{
					TSharedRef<SButton> Button = SNew(SButton);
					Button->SetHAlign(HAlign_Center);
					Button->SetVAlign(VAlign_Center);
					Button->SetToolTipText(LOCTEXT("unveil_tile", "Click to unveil this tile"));
					SUniformGridPanel::FSlot& Slot = MineGrid->AddSlot(CurCol, CurRow);
					Slot.AttachWidget(Button);
					Button->SetOnClicked(FOnClicked::CreateLambda([this, CurCol, CurRow, RowCount, ColCount, BombCount]()
						{
							UnveilTile(CurCol, CurRow, RowCount, ColCount, BombCount);
							return FReply::Handled();
						}
					));
				}
			}
		}
	}
	else
	{
		Debug("Pointers to slate widgets are invalid");
	}
	return FReply::Handled();
}

void SMinesweeperDockTab::InitializeGame(int32 Height, int32 Width, int32 BombCount, int32 InitIdx)
{
	InfoBlock->SetText(FText::FromString("Good luck ! Have fun"));
	BombLocations.Init(false, Height * Width);
	while (BombCount)
	{
		int BombIdx = FMath::RandRange(0, (Height * Width) - 1);
		if (BombLocations[BombIdx] == false && BombIdx != InitIdx)
		{
			// not initial click and empty
			BombLocations[BombIdx] = true;
			BombCount--;
		}
	}
	bIsGameInitialized = true;
}

void SMinesweeperDockTab::Debug(FString Text)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Text);
	}
	UE_LOG(LogTemp, Warning, TEXT("Debug Minesweeper: <%s>"), *Text);
}

void SMinesweeperDockTab::UnveilTile(int32 CurCol, int32 CurRow, int32 RowCount, int32 ColCount, int32 BombCount)
{
	const int32 ClickIdx = (CurRow * ColCount) + CurCol;
	if (bIsGameOver || ClickIdx >= MineGrid->GetChildren()->Num())
	{
		return;
	}
	// cast to SButton from SWidget
	TSharedRef<SButton> Button = StaticCastSharedRef<SButton, SWidget>(MineGrid->GetChildren()->GetChildAt(ClickIdx));
	if (!Button->IsEnabled())
	{
		// recursive escape
		return;
	}
	Button->SetEnabled(false);
	Button->SetToolTipText(FText::FromString(""));
	TilesLeft--;
	if (!bIsGameInitialized)
	{
		// first click
		InitializeGame(RowCount, ColCount, BombCount, ClickIdx);
	}
	else if (BombLocations.IsValidIndex(ClickIdx) && BombLocations[ClickIdx])
	{
		// game over
		InfoBlock->SetText(LOCTEXT("game_over", "BOOOOOOM ! Game over"));
		InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		Button->SetBorderBackgroundColor(FLinearColor::Red);
		Button->SetToolTipText(FText::FromString("BOMB"));
		bIsGameOver = true;
	}
	// check adjacent tiles
	if (!bIsGameOver)
	{
		// count adjacent mines
		int32 AdjacentMineCount = 0;
		for (int32 r = -1; r < 2; r++)
			for (int32 c = -1; c < 2; c++)
			{
				if ((CurRow + r) >= 0 && (CurCol + c) >= 0 && (CurRow + r) < RowCount && (CurCol + c) < ColCount)
				{
					int32 AdjacentIdx = (CurRow + r) * ColCount + (CurCol + c);
					if (BombLocations[AdjacentIdx])
						AdjacentMineCount++;
				}
			}
		if (AdjacentMineCount > 0)
		{
			// display mine count label
			TSharedRef<STextBlock> AdjacentMineLabel = SNew(STextBlock);
			UFont* Font = LoadObject<UFont>(nullptr, TEXT("/Engine/EngineFonts/Roboto.Roboto"), nullptr, LOAD_None, nullptr);
			AdjacentMineLabel->SetFont(FSlateFontInfo((UObject*)Font, 20));
			AdjacentMineLabel->SetText(FText::FromString(FString::FromInt(AdjacentMineCount)));
			Button->SetContent(AdjacentMineLabel);
		}
		else
		{
			// expand zone recusively
			Button->SetBorderBackgroundColor(FLinearColor::Blue);
			for (int32 r = -1; r < 2; r++)
				for (int32 c = -1; c < 2; c++)
				{
					if ((CurRow + r) >= 0 && (CurCol + c) >= 0 && (CurRow + r) < RowCount && (CurCol + c) < ColCount)
					{
						UnveilTile(CurCol + c, CurRow + r, RowCount, ColCount, BombCount);
					}
				}
		}
		// check win
		if (TilesLeft == BombCount)
		{
			InfoBlock->SetText(LOCTEXT("game_win", "It's a win ! Congratulations"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			bIsGameOver = true;
		}
	}
}

#undef LOCTEXT_NAMESPACE