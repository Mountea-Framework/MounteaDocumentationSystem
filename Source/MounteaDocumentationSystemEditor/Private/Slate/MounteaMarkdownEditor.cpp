﻿// All rights reserved Dominik Morse 2024

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
				.OnChildTextChanged(this, &SMounteaMarkdownEditor::HandleChildTextChanged)
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

	FString fullText = EditedPage->PageContent.ToString();
	float wrapWidth = EditableTextWidget->GetCachedGeometry().GetLocalSize().X - 40.f;
	if (wrapWidth <= 0.f) wrapWidth = 800.f;

	auto fontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FSlateFontInfo fontInfo = GetEditorFont();

	TArray<FString> hardLines;
	fullText.ParseIntoArray(hardLines, TEXT("\n"), false);

	int32 totalWrappedLines = 0;
	for (const FString& hardLine : hardLines)
	{
		TArray<FString> words;
		hardLine.ParseIntoArray(words, TEXT(" "), true);

		FString currentSoftLine;
		for (int32 iWord = 0; iWord < words.Num(); ++iWord)
		{
			FString prefix = currentSoftLine.IsEmpty() ? TEXT("") : TEXT(" ");
			FString newLine = currentSoftLine + prefix + words[iWord];
			float newWidth = fontMeasure->Measure(newLine, fontInfo).X;
			if (!currentSoftLine.IsEmpty() && newWidth > wrapWidth)
			{
				++totalWrappedLines;
				currentSoftLine = words[iWord];
			}
			else
			{
				currentSoftLine = newLine;
			}
		}
		++totalWrappedLines;
	}
	return totalWrappedLines - (totalWrappedLines * 0.05f); //ugliest thing I have done in while :)
}

FText SMounteaMarkdownEditor::GetLineNumbers() const
{
	if (!EditableTextWidget.IsValid()) return FText::GetEmpty();

	int32 totalLines = CalculateManualWrappedLineCount();
	FString lineNumbers;
	for (int32 i = 1; i <= totalLines; i++)
	{
		lineNumbers += FString::Printf(TEXT("%d\n"), i);
	}
	return FText::FromString(lineNumbers);
}

void SMounteaMarkdownEditor::HandleChildTextChanged(const FText& NewText)
{
	SetText(NewText);
	
	ConvertMarkdownToRichText();
}

void SMounteaMarkdownEditor::ConvertMarkdownToRichText()
{
	if (!EditedPage.IsValid()) return;

	FString text = EditedPage->PageContent.ToString();

	//TODO: headers should apply to whole row
	
	FormatTextWithTags(text, TEXT("_"), TEXT("_"), TEXT("<RichTextBlock.Italic>"), TEXT("</>"));
	FormatTextWithTags(text, TEXT("_"), TEXT("_"), TEXT("<RichTextBlock.Italic>"), TEXT("</>"));
	FormatTextWithTags(text, TEXT("**"), TEXT("**"), TEXT("<RichTextBlock.BoldHighlight>"), TEXT("</>"));

	// todo: different font, mono!
	FormatTextWithTags(text, TEXT("`"), TEXT("`"), TEXT("<RichTextBlock.TextHighlight>"), TEXT("</>"));

	EditedPage->RichTextPageContent = FText::FromString(text);
}

void SMounteaMarkdownEditor::FormatTextWithTags(
	FString& source,
	const FString& startMarker,
	const FString& endMarker,
	const FString& startTag,
	const FString& endTag
)
{
	int32 searchIndex = 0;
	while (true)
	{
		int32 openPos = source.Find(startMarker, ESearchCase::CaseSensitive, ESearchDir::FromStart, searchIndex);
		if (openPos == INDEX_NONE)
		{
			break;
		}

		int32 closePos = source.Find(endMarker, ESearchCase::CaseSensitive, ESearchDir::FromStart, openPos + startMarker.Len());
		if (closePos == INDEX_NONE || closePos <= openPos)
		{
			break;
		}

		int32 contentStart = openPos + startMarker.Len();
		int32 contentLen = closePos - contentStart;
		if (contentLen <= 0)
		{
			break;
		}

		FString inner = source.Mid(contentStart, contentLen);
		FString replaced = startTag + inner + endTag;
		source = source.Left(openPos) + replaced + source.Mid(closePos + endMarker.Len());

		searchIndex = openPos + replaced.Len();
	}
}

