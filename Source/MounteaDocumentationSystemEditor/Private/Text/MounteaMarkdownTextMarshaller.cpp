// All rights reserved Dominik Morse 2024


#include "MounteaMarkdownTextMarshaller.h"
#include "Framework/Text/SlateTextRun.h"
#include "Settings/MounteaDocumentationSystemEditorSettings.h"

TSharedRef<FMarkdownTextMarshaller> FMarkdownTextMarshaller::Create()
{
	return MakeShared<FMarkdownTextMarshaller>();
}

FMarkdownTextMarshaller::FMarkdownTextMarshaller()
	: FPlainTextLayoutMarshaller() {}

void FMarkdownTextMarshaller::SetText(const FString& SourceString, FTextLayout& TargetTextLayout)
{
	TargetTextLayout.ClearLines();
	TArray<FTextLayout::FNewLineData> LinesToAdd;

	TArray<FTextRange> LineRanges;
	FTextRange::CalculateLineRangesFromString(SourceString, LineRanges);
	
	const UMounteaDocumentationSystemEditorSettings* EditorSettings = GetDefault<UMounteaDocumentationSystemEditorSettings>();
	const FSlateFontInfo DefaultFont = EditorSettings->GetEditorFont();

	for (const FTextRange& LineRange : LineRanges)
	{
		TSharedRef<FString> LineText = MakeShared<FString>(SourceString.Mid(LineRange.BeginIndex, LineRange.Len()));
		TArray<TSharedRef<IRun>> Runs;
		
		FSlateFontInfo CustomFont = DefaultFont;
		FLinearColor TextColor = FLinearColor::White;
		const FLinearColor highlightedColor = FLinearColor(FColor::FromHex("569cd6"));

		// Markdown syntax highlighting while keeping the user's font
		if (LineText->StartsWith("# "))
		{
			TextColor = highlightedColor;
		}
		else if (LineText->StartsWith("**") && LineText->EndsWith("**"))
		{
			TextColor = highlightedColor;
		}
		else if (LineText->StartsWith("_") && LineText->EndsWith("_"))
		{
			TextColor = highlightedColor;
		}
		else if (LineText->Contains("[") && LineText->Contains("]("))
		{
			TextColor = highlightedColor;
		}
		
		FTextBlockStyle TextStyle;
		TextStyle.SetFont(CustomFont);
		TextStyle.SetColorAndOpacity(TextColor);

		Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, TextStyle));
		LinesToAdd.Emplace(MoveTemp(LineText), MoveTemp(Runs));
	}

	TargetTextLayout.AddLines(LinesToAdd);
}

void FMarkdownTextMarshaller::GetText(FString& TargetString, const FTextLayout& SourceTextLayout)
{
	SourceTextLayout.GetAsText(TargetString);
}
