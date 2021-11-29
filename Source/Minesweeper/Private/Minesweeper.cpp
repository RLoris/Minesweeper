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
					.Text(LOCTEXT("minecount", "Mine count"))
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
						.Text(LOCTEXT("generategrid", "Generate grid"))
					]
				]
				/*, 
				*/
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
	if (WidthBox.IsValid() && HeightBox.IsValid() && BombBox.IsValid() && MineGrid.IsValid())
	{
		FString debug = WidthBox->GetText().ToString() + " " + HeightBox->GetText().ToString() + " " + BombBox->GetText().ToString();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, debug);
		}
	}
	return FReply::Handled();
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