// Copyright Epic Games, Inc. All Rights Reserved.

#include "Minesweeper.h"
#include "MinesweeperStyle.h"
#include "MinesweeperCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
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
				SAssignNew(InfoBlock, STextBlock)
			]
			+ SVerticalBox::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center).Padding(5).FillHeight(1.0)
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
		int32 Width = FCString::Atoi(*WidthBox->GetText().ToString());
		int32 Height = FCString::Atoi(*HeightBox->GetText().ToString());
		int32 BombCount = FCString::Atoi(*BombBox->GetText().ToString());
		if (Width <= 0 || Height <= 0 || BombCount <= 0)
		{
			InfoBlock->SetText(LOCTEXT("invalid_input", "The Width / Height / Mine Count is invalid !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else if (Width > 15 || Height > 15)
		{
			InfoBlock->SetText(LOCTEXT("invalid_size", "The Width or Height is greater than 15 !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
		else if (BombCount > (Height * Width))
		{
			InfoBlock->SetText(LOCTEXT("invalid_count", "The bombcount is greater than the size of the grid !"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
		else
		{
			InfoBlock->SetText(LOCTEXT("start_game", "Click on a tile to start"));
			InfoBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			MineGrid->ClearChildren();
			for (int i = 0; i < Width; i++)
			{
				for (int j = 0; j < Height; j++)
				{
					auto Button = SNew(SButton);
					Button->SetTag(FName(FString::FromInt(j) + "-" + FString::FromInt(i)));
					Button->SetToolTipText(LOCTEXT("unveil_tile", "Click to unveil this tile"));
					auto& Slot = MineGrid->AddSlot(j, i);
					Slot.AttachWidget(Button);
					
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

void FMinesweeperModule::Debug(FString Text)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Text);
	}
	UE_LOG(LogTemp, Warning, TEXT("Debug Minesweeper: <%s>"), *Text);
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