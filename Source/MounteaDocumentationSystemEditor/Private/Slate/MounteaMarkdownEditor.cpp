// All rights reserved Dominik Morse 2024

#include "MounteaMarkdownEditor.h"

#include "MounteaMarkdownTextEditor.h"
#include "Core/MounteaDocumentationPage.h"
#include "Settings/MounteaDocumentationSystemEditorSettings.h"

void SMounteaMarkdownEditor::Construct(const FArguments& InArgs)
{
	EditedPage = InArgs._EditedPage;
	UpdateMarkdownEditor();
}

BEGIN_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::UpdateMarkdownEditor()
{
	ChildSlot
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
				.Text(this, &SMounteaMarkdownEditor::GetLineNumbers)
				.Justification(ETextJustify::Right)
				.ColorAndOpacity(this, &SMounteaMarkdownEditor::GetLineNumberColor)
			]
		]

		// Markdown Input (Editor)
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(SBorder)
			.Padding(10)
			.BorderBackgroundColor(FLinearColor::Transparent)
			[
				SAssignNew(EditableTextWidget, SMounteaMarkdownTextEditor)
				.EditedPage(EditedPage)
				.EditorFont(this, &SMounteaMarkdownEditor::GetEditorFont)
			]
		]
	];
}

END_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime,
	const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (EditableTextWidget.IsValid())
	{
		EditableTextWidget->UpdateEditorFont(GetEditorFont());
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}	
}

FText SMounteaMarkdownEditor::GetText() const
{
	return EditedPage.IsValid() ? EditedPage->PageContent : FText::GetEmpty();
}

void SMounteaMarkdownEditor::SetText(const FText& NewText)
{
	if (EditedPage.IsValid())
		EditedPage->PageContent = NewText;
}

FText SMounteaMarkdownEditor::GetLineNumbers() const
{
	auto editorSettings = GetMutableDefault<UMounteaDocumentationSystemEditorSettings>();
	
	if (!IsValid(EditedPage.Get()) || !EditableTextWidget.IsValid())
		return FText::GetEmpty();

	const FString markdownText = EditedPage->PageContent.ToString();
	TArray<FString> Lines;
	markdownText.ParseIntoArray(Lines, TEXT("\n"), false);

	float WrapWidth = EditableTextWidget->GetCachedGeometry().GetLocalSize().X;
	if (WrapWidth <= 0) WrapWidth = 800.0f;

	/*
	const float fontSize = EditableTextWidget->GetFont().Size;
	constexpr float baseFontSize = 10.0f;
	constexpr float baseCharWidth = 6.0f;
	*/
	const float estimatedCharWidth = editorSettings->EstimatedCharWidth;

	int32 wrappedLineCount = 0;
	for (const FString& Line : Lines)
	{
		int32 lineLength = Line.Len();
		float estimatedCharsPerLine = WrapWidth / estimatedCharWidth;
		wrappedLineCount += FMath::CeilToInt(lineLength / estimatedCharsPerLine);
	}

	const int32 estimatedTotalLines = FMath::Max(wrappedLineCount, EditableTextWidget->GetTextLineCount());

	FString lineNumbers;
	for (int32 i = 1; i <= estimatedTotalLines; i++)
	{
		lineNumbers += FString::Printf(TEXT("%d\n"), i);
	}

	return FText::FromString(lineNumbers);
}

FSlateFontInfo SMounteaMarkdownEditor::GetEditorFont() const
{
	auto editorSettings = GetMutableDefault<UMounteaDocumentationSystemEditorSettings>();
	return editorSettings->GetEditorFont();
}

FSlateColor SMounteaMarkdownEditor::GetLineNumberColor() const
{
	return FLinearColor(1.f, 1.f, 1.f ,0.4f);
}

FReply SMounteaMarkdownEditor::HandleTabPress(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	if (KeyEvent.GetKey() == EKeys::Tab)
	{
		if (EditableTextWidget.IsValid())
			EditableTextWidget->InsertTextAtCursor(TEXT("\t"));

		return FReply::Handled();
	}
	return FReply::Unhandled();
}

