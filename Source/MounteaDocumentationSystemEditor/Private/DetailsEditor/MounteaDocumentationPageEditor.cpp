// All rights reserved Dominik Morse 2024


#include "DetailsEditor/MounteaDocumentationPageEditor.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"
#include "Core/MounteaDocumentationPage.h"
#include "Widgets/Text/SMultiLineEditableText.h"

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
				->SetSizeCoefficient(0.3f)
				->AddTab(MounteaDocumentationPropertyID, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split(
				// Right side: Markdown Editor + Preview
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
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
	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			SNew(SHorizontalBox)

			// Markdown Input
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[
				SNew(SMultiLineEditableText)
				.Text_Lambda([this]() -> FText { return IsValid(EditedPage) ? EditedPage->PageBody : FText::GetEmpty(); })
				.OnTextChanged_Lambda([this](const FText& NewText)
				{
					if (IsValid(EditedPage))
					{
						EditedPage->PageBody = NewText;
						EditedPage->MarkdownPageBody = FText::FromString(TEXT("[Rendered Markdown]")); // TODO: Process Markdown
					}
				})
			]

			// Rendered Markdown Preview
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SNew(STextBlock)
					.AutoWrapText(true)
					.Text_Lambda([this]() -> FText { return IsValid(EditedPage) ? EditedPage->MarkdownPageBody : FText::GetEmpty(); })
				]
			]
		];
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

