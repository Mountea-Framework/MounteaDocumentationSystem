// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"

#include "CoreMinimal.h"
#include "Framework/Text/ITextDecorator.h"

class FMounteaMarkdownDecorator : public ITextDecorator
{
public:
	static TSharedRef<FMounteaMarkdownDecorator> Create();
    
	virtual ~FMounteaMarkdownDecorator() {}
    
	// ITextDecorator interface
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;
	virtual TSharedRef<ISlateRun> Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style) override;

private:
	FMounteaMarkdownDecorator() {}
};
