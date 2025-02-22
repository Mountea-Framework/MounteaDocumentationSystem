// All rights reserved Dominik Morse 2024


#include "MounteaDocumentationSystem/Public/TextDecorators/MounteaRichTextDecorators.h"

#include "Framework/Text/SlateTextRun.h"
#include "Settings/MounteaDocumentationSystemSettings.h"

TSharedRef<ISlateRun> FMounteaRichTextDecorator::Create(const TSharedRef<class FTextLayout>& TextLayout,
	const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText,
	const ISlateStyle* Style)
{
	// Use monospace font
	FTextBlockStyle CodeTextStyle = Style->GetWidgetStyle<FTextBlockStyle>("NormalText");
	CodeTextStyle.SetFont(FCoreStyle::GetDefaultFontStyle("UbuntuMono", 15));
	return FSlateTextRun::Create(FRunInfo(), InOutModelText, CodeTextStyle);
}

bool FMounteaRichTextDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	return false;
}

/*
TSharedRef<ISlateRun> FMounteaLinkDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
	const FTextRange* const MetaDataHrefRange = RunParseResult.MetaData.Find(TEXT("href"));
	if (!MetaDataHrefRange)
	{
		return FSlateTextRun::Create(RunInfo, InOutModelText, Style->GetWidgetStyle<FTextBlockStyle>("NormalText"), ModelRange);
	}

	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
	ModelRange.EndIndex = InOutModelText->Len();

	FRunInfo RunInfo(RunParseResult.Name);
	for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
	{
		RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
	}

	return FSlateHyperlinkRun::Create(RunInfo, InOutModelText, Style->GetWidgetStyle<FHyperlinkStyle>("Hyperlink"), OnLinkClicked, OnGetTooltip, FSlateHyperlinkRun::FOnGenerateTooltip(), ModelRange);
}

bool FMounteaLinkDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	return RunParseResult.Name == TEXT("a");
}
*/

bool UMounteaCodeDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	if (RunParseResult.Name != TEXT("text")) return false;
    
	const int32 ContentLen = RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex;
	if (ContentLen <= 0) return false;
    
	return Text[RunParseResult.ContentRange.BeginIndex] == MounteaTextTags::CodeMarker
		&& Text[RunParseResult.ContentRange.EndIndex - 1] == MounteaTextTags::CodeMarker;
}

TSharedRef<ISlateRun> UMounteaCodeDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
    
	// Extract content without markers
	const FString Content = OriginalText.Mid(RunParseResult.ContentRange.BeginIndex + 1, 
		RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex - 2);
	*InOutModelText += Content;
	ModelRange.EndIndex = InOutModelText->Len();

	// Use monospace font
	FTextBlockStyle CodeTextStyle = Style->GetWidgetStyle<FTextBlockStyle>("NormalText");
	CodeTextStyle.SetFont(FCoreStyle::GetDefaultFontStyle("UbuntuMono", 15));

	return FSlateTextRun::Create(FRunInfo(), InOutModelText, CodeTextStyle, ModelRange);
}

TSharedRef<ISlateRun> UMounteaCodeBlockDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
	ModelRange.EndIndex = InOutModelText->Len();

	FRunInfo RunInfo(RunParseResult.Name);
	for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
	{
		RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
	}

	return FSlateTextRun::Create(RunInfo, InOutModelText, CodeBlockStyle, ModelRange);
}

bool UMounteaCodeBlockDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	return RunParseResult.Name == TEXT("pre");
}

TSharedRef<ISlateRun> UMounteaItalicDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex,
		RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
	ModelRange.EndIndex = InOutModelText->Len();

	// Use registered style or fallback to italic font
	if(Style->HasWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.Italic")))
	{
		return FSlateTextRun::Create(FRunInfo(), InOutModelText, 
			Style->GetWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.Italic")), 
			ModelRange);
	}

	auto displaySettings = GetMutableDefault<UMounteaDocumentationSystemSettings>();
	auto newConfig = displaySettings->FontMappings.Find(TEXT("Italic"));
	

	FTextBlockStyle mounteaItalicStyle = Style->GetWidgetStyle<FTextBlockStyle>("NormalText");
	mounteaItalicStyle.SetFont(newConfig->PreviewFont);
	return FSlateTextRun::Create(FRunInfo(), InOutModelText, mounteaItalicStyle, ModelRange);
}


