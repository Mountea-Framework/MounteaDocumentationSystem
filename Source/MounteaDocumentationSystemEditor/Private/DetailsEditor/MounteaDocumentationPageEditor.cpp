// All rights reserved Dominik Morse 2024


#include "DetailsEditor/MounteaDocumentationPageEditor.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"
#include "Core/MounteaDocumentationPage.h"
#include "Settings/MounteaDocumentationSystemEditorSettings.h"
#include "Slate/MounteaMarkdownEditor.h"
#include "TextDecorators/MounteaRichTextDecorators.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Style/MounteaDocumentationStyle.h"

#define LOCTEXT_NAMESPACE "MounteaDocumentationEditor"

const FName MounteaDocumentationPageTabName = FName(TEXT("MounteaDocumentationPageEditorApp"));
const FName  MounteaDocumentationPropertyID(TEXT(" MounteaDocumentationPageProperty"));

const FName MarkdownTabId(TEXT("MounteaDocumentationEditor_Markdown"));

void FMounteaDocumentationPageEditor::InitMounteaDocumentationEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object)
{
	EditedPage = Cast<UMounteaDocumentationPage>(Object);

	FDetailsViewArgs Args; //( false, false, true, FDetailsViewArgs::HideNameArea, false );
	Args.bUpdatesFromSelection = false;
	Args.bLockable = false;
	Args.bAllowSearch = true;
	Args.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	Args.bHideSelectionTip = false;
	Args.bShowObjectLabel = false;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditedPage);

	PropertyWidget->OnFinishedChangingProperties().AddSP(
		this, &FMounteaDocumentationPageEditor::OnFinishedChangingProperties);

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_MounteaDocumentationEditor_LayoutV1.0")
	->AddArea(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split(
			FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
			->SetSizeCoefficient(1.0f) // Full editor window
			->Split(
				// Left side: Details Panel
				FTabManager::NewStack()
				->SetSizeCoefficient(0.15f)
				->AddTab(MounteaDocumentationPropertyID, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split(
				// Right side: Markdown Editor + Preview
				FTabManager::NewStack()
				->SetSizeCoefficient(0.85f)
				->AddTab(MarkdownTabId, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
		)
	);
	
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor
	(
		Mode,
		InitToolkitHost,
		MounteaDocumentationPageTabName,
		StandaloneDefaultLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		EditedPage,
		false
	);
}

void FMounteaDocumentationPageEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(
		LOCTEXT("WorkspaceMenu_MounteaDocumentationPageEditor", "Mountea Documentation Page Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	InTabManager->RegisterTabSpawner(MarkdownTabId, FOnSpawnTab::CreateSP(this, &FMounteaDocumentationPageEditor::SpawnMarkdownTab))
		.SetDisplayName(LOCTEXT("MarkdownTab", "Markdown"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner(MounteaDocumentationPropertyID,
									FOnSpawnTab::CreateSP(this, &FMounteaDocumentationPageEditor::SpawnDetailsTab))
				.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
				.SetGroup(WorkspaceMenuCategoryRef)
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FMounteaDocumentationPageEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(MarkdownTabId);
}

TSharedRef<SDockTab> FMounteaDocumentationPageEditor::SpawnMarkdownTab(const FSpawnTabArgs& Args)
{
	TArray<TSharedRef<ITextDecorator>> Decorators;
	Decorators.Add(MakeShared<UMounteaBoldDecorator>());
	Decorators.Add(MakeShared<UMounteaItalicDecorator>());
	Decorators.Add(MakeShared<UMounteaCodeDecorator>());
	Decorators.Add(MakeShared<UMounteaCodeBlockDecorator>());
	
	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNew(SBorder)
					.Padding(0)
					.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
				]

				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNew(SBorder)
					.Padding(0)
					.BorderImage(FAppStyle::GetBrush("Brushes.Background"))
				]
			]

			+ SOverlay::Slot()
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)

				+ SScrollBox::Slot()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(0.5f)
					[
						SNew(SBorder)
						.Padding(10)
						.BorderBackgroundColor(FLinearColor::Transparent)
						[
							SNew(SMounteaMarkdownEditor)
							.EditedPage(EditedPage)
						]
					]

					+ SHorizontalBox::Slot()
					.FillWidth(0.5f)
					[
						SNew(SBorder)
						.Padding(25)
						.BorderBackgroundColor(FLinearColor::Transparent)
						[
							SAssignNew(PreviewWindow, SRichTextBlock)
							.AutoWrapText(true)
							.Text_Lambda([this]() -> FText { return IsValid(EditedPage) ? EditedPage->RichTextPageContent : FText::GetEmpty(); })
							.TextStyle(&FMounteaDocumentationStyle::Get(), "RichTextBlock.Mountea.Regular")
							.Decorators( Decorators )
							.DecoratorStyleSet(&FMounteaDocumentationStyle::Get())
							.HighlightText_Lambda([this]() -> FText
							{
								return IsValid(EditedPage) ? GetText("RichTextBlock.Mountea.Code") : FText::GetEmpty();
							})
						]
					]
				]
			]
		];
}

FText FMounteaDocumentationPageEditor::GetText(const FString& StyleName) const
{
	if (!IsValid(EditedPage)) return FText::GetEmpty();
	
	FString Content = EditedPage->RichTextPageContent.ToString();
	FString StartTag = FString::Printf(TEXT("<%s>"), *StyleName);
	FString EndTag = TEXT("</>");
    
	// Find all text between style tags
	FString Result;
	int32 StartIndex = Content.Find(StartTag);
	while (StartIndex != INDEX_NONE)
	{
		int32 EndIndex = Content.Find(EndTag, ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIndex);
		if (EndIndex != INDEX_NONE)
		{
			StartIndex += StartTag.Len();
			Result += Content.Mid(StartIndex, EndIndex - StartIndex) + TEXT(" ");
		}
		StartIndex = Content.Find(StartTag, ESearchCase::CaseSensitive, ESearchDir::FromStart, EndIndex);
	}
	return FText::FromString(Result);
}

TSharedRef<SDockTab> FMounteaDocumentationPageEditor::SpawnDetailsTab(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == MounteaDocumentationPropertyID);

	auto DockTab = SNew(SDockTab)
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];

	DockTab->SetTabIcon(FAppStyle::GetBrush("LevelEditor.Tabs.Details"));
	return DockTab;
}

void FMounteaDocumentationPageEditor::OnFinishedChangingProperties(
	const FPropertyChangedEvent& PropertyChangedEvent)
{
	// TODO: Rebuid Markdown etc
}

FName FMounteaDocumentationPageEditor::GetToolkitFName() const { return FName("MounteaDocumentationEditor"); }
FText FMounteaDocumentationPageEditor::GetBaseToolkitName() const { return LOCTEXT("AppLabel", "Mountea Documentation Editor"); }
FText FMounteaDocumentationPageEditor::GetToolkitToolTipText() const { return LOCTEXT("ToolTip", "Editor for Mountea Documentation Pages"); }
FLinearColor FMounteaDocumentationPageEditor::GetWorldCentricTabColorScale() const { return FLinearColor::White; }
FString FMounteaDocumentationPageEditor::GetWorldCentricTabPrefix() const { return TEXT("MounteaDocumentationEditor"); }

#undef LOCTEXT_NAMESPACE

