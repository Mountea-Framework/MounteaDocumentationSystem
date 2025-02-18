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
	SpawnEditorTextWidget();

	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			SNew(SOverlay)

			// Background Layer
			+ SOverlay::Slot()
			[
				SNew(SHorizontalBox)

				// Editor Background
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNew(SBorder)
					.Padding(0)
					.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
				]

				// Preview Background
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNew(SBorder)
					.Padding(0)
					.BorderImage(FAppStyle::GetBrush("Brushes.Background"))
				]
			]

			// Foreground Layer (Scrollable Content)
			+ SOverlay::Slot()
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)

				+ SScrollBox::Slot()
				[
					SNew(SHorizontalBox)

					// Markdown Editor (With Line Numbers)
					+ SHorizontalBox::Slot()
					.FillWidth(0.5f)
					[
						SNew(SBorder)
						.Padding(10)
						.BorderBackgroundColor(FLinearColor::Transparent)
						[
							SNew(SHorizontalBox)

							// Line Numbers Column
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SBorder)
								.Padding(10)
								.BorderBackgroundColor(FLinearColor::Transparent)
								[
									SNew(STextBlock)
									.Text_Lambda([this]() -> FText { return GenerateLineNumbers(); })
									.Justification(ETextJustify::Right)
									.ColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)))
								]
							]

							// Markdown Input (Editor)
							+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							[
								SNew(SBorder)
								.Padding(10)
								.BorderBackgroundColor(FLinearColor::Transparent)
								[
									EditableTextWidget.ToSharedRef()
								]
							]
						]
					]

					// Rendered Markdown Preview
					+ SHorizontalBox::Slot()
					.FillWidth(0.5f)
					[
						SNew(SBorder)
						.Padding(10)
						.BorderBackgroundColor(FLinearColor::Transparent)
						[
							SNew(STextBlock)
							.AutoWrapText(true)
							.Text_Lambda([this]() -> FText { return IsValid(EditedPage) ? EditedPage->MarkdownPageContent : FText::GetEmpty(); })
						]
					]
				]
			]
		];
}

FText FMounteaDocumentationPageEditor::GenerateLineNumbers() const
{
	if (!IsValid(EditedPage) || !EditableTextWidget.IsValid())
		return FText::GetEmpty();

	const FString markdownText = EditedPage->PageContent.ToString();
	TArray<FString> Lines;
	markdownText.ParseIntoArray(Lines, TEXT("\n"), false); // Preserve empty lines

	// Get the actual text wrapping width
	float WrapWidth = EditableTextWidget->GetCachedGeometry().GetLocalSize().X * 0.975f;
	if (WrapWidth <= 0) WrapWidth = 800.0f; // Fallback in case geometry isn't valid

	// Estimate wrapped lines dynamically
	int32 EstimatedWrappedLines = 0;
	for (const FString& Line : Lines)
	{
		int32 LineLength = Line.Len();
		EstimatedWrappedLines += FMath::CeilToInt(LineLength / (WrapWidth / 10.0f)); // Adjust division factor if needed
	}

	int32 TotalLines = FMath::Max(Lines.Num(), EstimatedWrappedLines);

	FString lineNumbers;
	for (int32 i = 1; i <= TotalLines; i++)
	{
		lineNumbers += FString::Printf(TEXT("%d\n"), i);
	}

	return FText::FromString(lineNumbers);
}

FReply FMounteaDocumentationPageEditor::HandleTabPress(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	if (KeyEvent.GetKey() == EKeys::Tab) // Detect Tab Key
	{
		if (IsValid(EditedPage))
			EditableTextWidget->InsertTextAtCursor(TEXT("\t"));

		return FReply::Handled(); // Prevents focus loss
	}

	return FReply::Unhandled();
}

void FMounteaDocumentationPageEditor::SpawnEditorTextWidget()
{
	EditableTextWidget = SNew(SMultiLineEditableText)
	.Text_Lambda([this]() -> FText { return IsValid(EditedPage) ? EditedPage->PageContent : FText::GetEmpty(); })
	.OnTextChanged_Lambda([this](const FText& NewText)
	{
		if (IsValid(EditedPage))
		{
			EditedPage->PageContent = NewText;
			EditedPage->MarkdownPageContent = FText::FromString(TEXT("[Rendered Markdown]")); // TODO: Process Markdown
		}
	})
	/*.WrapTextAt_Lambda([this]() -> float
	{
		return EditableTextWidget.IsValid() ? EditableTextWidget->GetCachedGeometry().GetLocalSize().X * 0.95f : 800.0f;
	})*/
	.AutoWrapText(true)
	.OnKeyDownHandler(this, &FMounteaDocumentationPageEditor::HandleTabPress);
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

