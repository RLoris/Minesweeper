// RLoris 2021

#include "Minesweeper.h"
#include "MinesweeperStyle.h"
#include "MinesweeperCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Engine/Font.h"
#include "ToolMenus.h"

static const FName MinesweeperTabName("Minesweeper");

#define LOCTEXT_NAMESPACE "FMinesweeperModule"

void FMinesweeperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMinesweeperStyle::Initialize();
	FMinesweeperStyle::ReloadTextures();

	FMinesweeperCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMinesweeperCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMinesweeperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MinesweeperTabName, FOnSpawnTab::CreateRaw(this, &FMinesweeperModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FMinesweeperTabTitle", "Minesweeper"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMinesweeperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMinesweeperStyle::Shutdown();

	FMinesweeperCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MinesweeperTabName);
}

TSharedRef<SDockTab> FMinesweeperModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	UFont* font = LoadObject<UFont>(nullptr, TEXT("/Engine/EngineFonts/Roboto.Roboto"), nullptr, LOAD_None, nullptr);
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
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
					SNew(SButton).HAlign(HAlign_Center)
					.OnClicked_Raw(this, &FMinesweeperModule::GenerateGrid)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("generate_grid", "Generate grid"))
					]
				]
				/*, 
				*/
			]
			+ SVerticalBox::Slot().HAlign(HAlign_Center).Padding(5).AutoHeight()
			[
				SAssignNew(InfoBlock, STextBlock).Font(FSlateFontInfo((UObject*) font, 32))
			]
			+ SVerticalBox::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(5).FillHeight(1.0)
			[
				SAssignNew(MineGrid, SUniformGridPanel)
			]
		];
}

void FMinesweeperModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MinesweeperTabName);
}

FReply FMinesweeperModule::GenerateGrid()
{
	if (WidthBox.IsValid() && HeightBox.IsValid() && BombBox.IsValid() && MineGrid.IsValid() && InfoBlock.IsValid())
	{
		int32 ColCount = FCString::Atoi(*WidthBox->GetText().ToString());
		int32 RowCount = FCString::Atoi(*HeightBox->GetText().ToString());
		int32 BombCount = FCString::Atoi(*BombBox->GetText().ToString());
		if (ColCount <= 0 || RowCount <= 0 || BombCount <= 0)
		{
			InfoBlock->SetText(LOCTEXT("invalid_input", "The Width / Height / Mine Count is invalid !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else if (ColCount > 16 || RowCount > 16)
		{
			InfoBlock->SetText(LOCTEXT("invalid_size", "The Width or Height is greater than 16 !"));
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
			IsGameInitialized = false;
			IsGameOver = false;
			for (int curRow = 0; curRow < RowCount; curRow++)
			{
				for (int curCol = 0; curCol < ColCount; curCol++)
				{
					auto Button = SNew(SButton);
					Button->SetHAlign(HAlign_Center);
					Button->SetVAlign(VAlign_Center);
					Button->SetTag(FName(FString::FromInt(curRow) + "-" + FString::FromInt(curCol)));
					Button->SetToolTipText(LOCTEXT("unveil_tile", "Click to unveil this tile"));
					auto& Slot = MineGrid->AddSlot(curCol, curRow);
					Slot.AttachWidget(Button);
					Button->SetOnClicked(FOnClicked::CreateLambda([this, curCol, curRow, RowCount, ColCount, BombCount]()
						{
							UnveilTile(curCol, curRow, RowCount, ColCount, BombCount);
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

void FMinesweeperModule::InitializeGame(int32 Height, int32 Width, int32 BombCount, int32 InitIdx)
{
	InfoBlock->SetText(FText::FromString("Good luck ! Have fun"));
	BombLocations.Init(false, Height * Width);
	while (BombCount)
	{
		int spawnIdx = FMath::RandRange(0, (Height * Width) - 1);
		if (BombLocations[spawnIdx] == false && spawnIdx != InitIdx)
		{
			// not initial click
			BombLocations[spawnIdx] = true;
			BombCount--;
		}
	}
	IsGameInitialized = true;
}

void FMinesweeperModule::Debug(FString Text)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Text);
	}
	UE_LOG(LogTemp, Warning, TEXT("Debug Minesweeper: <%s>"), *Text);
}

void FMinesweeperModule::UnveilTile(int32 curCol, int32 curRow, int32 RowCount, int32 ColCount, int32 BombCount)
{
	const int32 clickIdx = (curRow * ColCount) + curCol;
	if (IsGameOver || clickIdx >= MineGrid->GetChildren()->Num())
	{
		return;
	}
	TSharedRef<SWidget> widget = MineGrid->GetChildren()->GetChildAt(clickIdx);
	SButton& Button = static_cast<SButton&>(widget.Get());
	if (!Button.IsEnabled())
	{
		// recursive escape
		return;
	}
	Button.SetEnabled(false);
	Button.SetToolTipText(FText::FromString(""));
	TilesLeft--;
	if (!IsGameInitialized)
	{
		// first click
		InitializeGame(RowCount, ColCount, BombCount, clickIdx);
	}
	else
	{
		// unveil
		if (BombLocations.IsValidIndex(clickIdx) && BombLocations[clickIdx])
		{
			// game over
			InfoBlock->SetText(LOCTEXT("game_over", "BOOOOOOM ! Game over"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
			Button.SetBorderBackgroundColor(FLinearColor::Red);
			Button.SetToolTipText(FText::FromString("BOMB"));
			IsGameOver = true;
		}
	}
	if (!IsGameOver)
	{
		// show mine count
		int mineCount = 0;
		for (int r = -1; r < 2; r++)
			for (int c = -1; c < 2; c++)
			{
				if ((curRow + r) >= 0 && (curCol + c) >= 0 && (curRow + r) < RowCount && (curCol + c) < ColCount)
				{
					int32 checkIdx = (curRow + r) * ColCount + (curCol + c);
					if (BombLocations[checkIdx])
						mineCount++;
				}
			}
		// display label
		if (mineCount > 0)
		{
			auto label = SNew(STextBlock);
			auto font = LoadObject<UFont>(nullptr, TEXT("/Engine/EngineFonts/Roboto.Roboto"), nullptr, LOAD_None, nullptr);
			label->SetFont(FSlateFontInfo((UObject*)font, 20));
			label->SetText(FText::FromString(FString::FromInt(mineCount)));
			Button.SetContent(label);
		}
		else
		{
			Button.SetBorderBackgroundColor(FLinearColor::Blue);
			// expand
			for (int r = -1; r < 2; r++)
				for (int c = -1; c < 2; c++)
				{
					if ((curRow + r) >= 0 && (curCol + c) >= 0 && (curRow + r) < RowCount && (curCol + c) < ColCount)
					{
						UnveilTile(curCol + c, curRow + r, RowCount, ColCount, BombCount);
					}
				}
		}
		// check win
		if (TilesLeft == BombCount)
		{
			InfoBlock->SetText(LOCTEXT("game_win", "It's a win ! Congratulations"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			IsGameOver = true;
		}
	}
}

void FMinesweeperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMinesweeperCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMinesweeperCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMinesweeperModule, Minesweeper)