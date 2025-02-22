// Copyright Dominik Morse. Published in 2024.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "Framework/Text/ISlateRun.h"


class UWidgetStudioText;

/**
 * Base class for all Mountea Documentation System decorators.
 * Provides common functionality and settings.
 */
class FMounteaRichTextDecorator : public ITextDecorator
{
public:
	static TSharedRef<FMounteaRichTextDecorator> Create()
	{
		return MakeShareable(new FMounteaRichTextDecorator());
	}

	virtual ~FMounteaRichTextDecorator()
	{
	}

	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
};

/**
 * Decorator for handling custom links in documentation.
 * Supports various protocols like asset://, settings://, etc.

UCLASS()
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaLinkDecorator : public FMounteaRichTextDecorator
{

public:
	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
protected:
	FSlateHyperlinkRun::FOnClick OnLinkClicked;
	
	FSlateHyperlinkRun::FOnGetTooltipText OnGetTooltip;
};
*/

/**
 * Decorator for inline code snippets.
 * Uses monospace font (UbuntuMono).
 */
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaCodeDecorator : public FMounteaRichTextDecorator
{

public:
	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		return RunParseResult.Name == TEXT("RichTextBlock.Mountea.Code");
	}
};

/**
 * Decorator for multi-line code blocks.
 * Uses monospace font (UbuntuMono) and adds background.
 */
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaCodeBlockDecorator : public UMounteaCodeDecorator
{

public:
	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		return RunParseResult.Name == TEXT("RichTextBlock.Mountea.CodeBlock");
	}
};

/**
 * Decorator for italic text.
 * Ensures proper font style is used.
 */
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaItalicDecorator : public FMounteaRichTextDecorator
{
public:
	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		return RunParseResult.Name == TEXT("RichTextBlock.Mountea.Italic");
	}
};

class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaBoldDecorator : public FMounteaRichTextDecorator
{
public:
	virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		return RunParseResult.Name == TEXT("RichTextBlock.Mountea.Bold");
	}
};
namespace MounteaTextTags
{
	static constexpr TCHAR ItalicMarker = TEXT('*');
	static constexpr TCHAR CodeMarker = TEXT('`');
	static constexpr TCHAR LinkOpenMarker = TEXT('<');
	static constexpr TCHAR LinkCloseMarker = TEXT('>');
    
	static constexpr const TCHAR* CodeBlockMarker = TEXT("```");
}