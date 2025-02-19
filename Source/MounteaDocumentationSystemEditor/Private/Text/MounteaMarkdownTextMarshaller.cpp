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

	UE_LOG(LogTemp, Warning, TEXT("Applying font and color to %d lines"), LineRanges.Num());

	for (int32 i = 0; i < LineRanges.Num(); i++)
	{
		const FTextRange& LineRange = LineRanges[i];
		FString LineText = SourceString.Mid(LineRange.BeginIndex, LineRange.Len());
		TSharedRef<FString> SharedLineText = MakeShared<FString>(LineText);
		TArray<TSharedRef<IRun>> Runs;

		// Determine the color
		FLinearColor TextColor = FLinearColor::White;
		const FLinearColor highlightColor = FLinearColor(FColor::FromHex("569cd6"));;
		if (LineText.StartsWith("# ")) // Headings
		{
			TextColor = highlightColor;
		}
		else if (LineText.StartsWith("**") && LineText.EndsWith("**")) // Bold
		{
			TextColor =  highlightColor;
		}
		else if (LineText.StartsWith("_") && LineText.EndsWith("_")) // Italic
		{
			TextColor =  highlightColor;
		}
		else if (LineText.Contains("[") && LineText.Contains("](")) // Links
		{
			TextColor =  highlightColor;
		}

		UE_LOG(LogTemp, Warning, TEXT("Line %d: '%s' -> Color: R=%.2f G=%.2f B=%.2f"),
			i + 1, *LineText, TextColor.R, TextColor.G, TextColor.B);

		FTextBlockStyle TextStyle;
		TextStyle.SetFont(DefaultFont);
		TextStyle.SetColorAndOpacity(TextColor);

		TSharedRef<FSlateTextRun> TextRun = FSlateTextRun::Create(FRunInfo(), SharedLineText, TextStyle);
		Runs.Add(TextRun);

		LinesToAdd.Emplace(SharedLineText, MoveTemp(Runs));
	}

	UE_LOG(LogTemp, Warning, TEXT("Finished processing %d lines"), LinesToAdd.Num());

	TargetTextLayout.AddLines(LinesToAdd);
}

void FMarkdownTextMarshaller::GetText(FString& TargetString, const FTextLayout& SourceTextLayout)
{
	SourceTextLayout.GetAsText(TargetString);
}
