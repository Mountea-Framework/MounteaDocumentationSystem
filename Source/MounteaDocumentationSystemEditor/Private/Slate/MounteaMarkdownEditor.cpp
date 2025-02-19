// All rights reserved Dominik Morse 2024

#include "MounteaMarkdownEditor.h"

#include "MounteaMarkdownTextEditor.h"
#include "Core/MounteaDocumentationPage.h"
#include "Fonts/FontMeasure.h"
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
				.Font(GetEditorFont())
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
		const float WrapWidth = AllottedGeometry.GetLocalSize().X - 40.0f;
		EditableTextWidget->SetWrapTextAt(WrapWidth);
		
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

FSlateFontInfo SMounteaMarkdownEditor::GetEditorFont() const
{
	auto editorSettings = GetMutableDefault<UMounteaDocumentationSystemEditorSettings>();
	return editorSettings->GetEditorFont();
}

FSlateColor SMounteaMarkdownEditor::GetLineNumberColor() const
{
	return FLinearColor(1.f, 1.f, 1.f ,0.4f);
}

int32 SMounteaMarkdownEditor::CalculateManualWrappedLineCount() const
{
	if (!EditedPage.IsValid() || !EditableTextWidget.IsValid()) return 0;
	
	FString FullText = EditedPage->PageContent.ToString();
	float WrapWidth = EditableTextWidget->GetCachedGeometry().GetLocalSize().X - 40.f;
	if (WrapWidth <= 0.f) WrapWidth = 800.f;
	
	auto FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const FSlateFontInfo FontInfo = GetEditorFont();
	TArray<FString> HardLines;
	FullText.ParseIntoArray(HardLines, TEXT("\n"), false);
	int32 TotalWrappedLines = 0;
	for (const FString& HardLine : HardLines)
	{
		TArray<FString> Words;
		HardLine.ParseIntoArray(Words, TEXT(" "), true);
		FString CurrentSoftLine;
		for (int32 iWord = 0; iWord < Words.Num(); ++iWord)
		{
			FString NextAttempt = CurrentSoftLine.IsEmpty() ? Words[iWord] : (CurrentSoftLine + TEXT(" ") + Words[iWord]);
			float MeasuredWidth = FontMeasure->Measure(NextAttempt, FontInfo).X;
			if (!CurrentSoftLine.IsEmpty() && MeasuredWidth > WrapWidth)
			{
				++TotalWrappedLines;
				CurrentSoftLine = Words[iWord];
			}
			else
				CurrentSoftLine = NextAttempt;
		}
		++TotalWrappedLines;
	}
	return TotalWrappedLines;
}

FText SMounteaMarkdownEditor::GetLineNumbers() const
{
	if (!EditableTextWidget.IsValid()) return FText::GetEmpty();
	int32 TotalLines = CalculateManualWrappedLineCount();
	FString LineNumbers;
	for (int32 i = 1; i <= TotalLines; i++)
	{
		LineNumbers += FString::Printf(TEXT("%d\n"), i);
	}
	return FText::FromString(LineNumbers);
}

