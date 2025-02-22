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

TSharedRef<ISlateRun> UMounteaCodeDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex,
		RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
	ModelRange.EndIndex = InOutModelText->Len();

	// Use registered style or fallback to Code font
	if(Style->HasWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.Code")))
	{
		return FSlateTextRun::Create(FRunInfo(), InOutModelText, 
			Style->GetWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.Code")), 
			ModelRange);
	}

	auto displaySettings = GetMutableDefault<UMounteaDocumentationSystemSettings>();
	auto newConfig = displaySettings->FontMappings.Find(TEXT("Code"));
	
	FTextBlockStyle mounteaItalicStyle = Style->GetWidgetStyle<FTextBlockStyle>("NormalText");
	mounteaItalicStyle.SetFont(newConfig->PreviewFont);
	return FSlateTextRun::Create(FRunInfo(), InOutModelText, mounteaItalicStyle, ModelRange);
}

TSharedRef<ISlateRun> UMounteaCodeBlockDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex,
		RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
	ModelRange.EndIndex = InOutModelText->Len();

	// Use registered style or fallback to Code font
	if(Style->HasWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.CodeBlock")))
	{
		return FSlateTextRun::Create(FRunInfo(), InOutModelText, 
			Style->GetWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.CodeBlock")), 
			ModelRange);
	}

	auto displaySettings = GetMutableDefault<UMounteaDocumentationSystemSettings>();
	auto newConfig = displaySettings->FontMappings.Find(TEXT("Code"));
	
	FTextBlockStyle mounteaItalicStyle = Style->GetWidgetStyle<FTextBlockStyle>("NormalText");
	mounteaItalicStyle.SetFont(newConfig->PreviewFont);
	return FSlateTextRun::Create(FRunInfo(), InOutModelText, mounteaItalicStyle, ModelRange);
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

TSharedRef<ISlateRun> UMounteaBoldDecorator::Create(const TSharedRef<class FTextLayout>& TextLayout,
	const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText,
	const ISlateStyle* Style)
{
	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	*InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex,
		RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
	ModelRange.EndIndex = InOutModelText->Len();

	// Use registered style or fallback to italic Bold
	if(Style->HasWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.Bold")))
	{
		return FSlateTextRun::Create(FRunInfo(), InOutModelText, 
			Style->GetWidgetStyle<FTextBlockStyle>(TEXT("RichTextBlock.Mountea.Bold")), 
			ModelRange);
	}

	auto displaySettings = GetMutableDefault<UMounteaDocumentationSystemSettings>();
	auto newConfig = displaySettings->FontMappings.Find(TEXT("Bold"));
	
	FTextBlockStyle mounteaItalicStyle = Style->GetWidgetStyle<FTextBlockStyle>("NormalText");
	mounteaItalicStyle.SetFont(newConfig->PreviewFont);
	return FSlateTextRun::Create(FRunInfo(), InOutModelText, mounteaItalicStyle, ModelRange);
}


