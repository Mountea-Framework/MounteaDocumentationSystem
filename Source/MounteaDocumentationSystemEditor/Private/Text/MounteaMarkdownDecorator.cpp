// All rights reserved Dominik Morse 2024


// MounteaMarkdownDecorator.cpp
#include "MounteaMarkdownDecorator.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/TextLayout.h"

TSharedRef<FMounteaMarkdownDecorator> FMounteaMarkdownDecorator::Create()
{
    return MakeShareable(new FMounteaMarkdownDecorator());
}

bool FMounteaMarkdownDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
    // Simple check for markdown syntax
    return Text.Contains(TEXT("#")) || 
           Text.Contains(TEXT("**")) || 
           Text.Contains(TEXT("_")) || 
           (Text.Contains(TEXT("[")) && Text.Contains(TEXT("](")));
}

TSharedRef<ISlateRun> FMounteaMarkdownDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, 
    const FTextRunParseResults& RunParseResult, 
    const FString& OriginalText, 
    const TSharedRef<FString>& InOutModelText, 
    const ISlateStyle* Style)
{
    // Get the current run's text
    const FString CurrentText = OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.Len());
    
    // Create text style for this run
    FTextBlockStyle TextStyle = FTextBlockStyle()
        .SetFont(FCoreStyle::Get().GetFontStyle("NormalFont"));
    
    // Simple markdown styling
    if (CurrentText.StartsWith(TEXT("# ")))
    {
        // Heading style
        TextStyle.SetColorAndOpacity(FLinearColor(FColor::FromHex("569cd6")));
    }
    else if (CurrentText.StartsWith(TEXT("**")) && CurrentText.EndsWith(TEXT("**")))
    {
        // Bold style
        TextStyle.SetColorAndOpacity(FLinearColor(FColor::FromHex("569cd6")));
    }
    else if (CurrentText.StartsWith(TEXT("_")) && CurrentText.EndsWith(TEXT("_")))
    {
        // Italic style
        TextStyle.SetColorAndOpacity(FLinearColor(FColor::FromHex("569cd6")));
    }
    else if (CurrentText.Contains(TEXT("[")) && CurrentText.Contains(TEXT("](")))
    {
        // Link style
        TextStyle.SetColorAndOpacity(FLinearColor(FColor::FromHex("569cd6")));
    }

    // Create and return the styled run
    TSharedRef<FString> TextToDisplay = MakeShared<FString>(CurrentText);
    return FSlateTextRun::Create(RunParseResult.Name, TextToDisplay, TextStyle);
}
