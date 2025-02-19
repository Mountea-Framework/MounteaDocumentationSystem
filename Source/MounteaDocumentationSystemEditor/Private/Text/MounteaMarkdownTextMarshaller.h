// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Framework/Text/PlainTextLayoutMarshaller.h"

class FMarkdownTextMarshaller : public FPlainTextLayoutMarshaller
{
public:
	static TSharedRef<FMarkdownTextMarshaller> Create();

	virtual void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override;
	virtual void GetText(FString& TargetString, const FTextLayout& SourceTextLayout) override;

public:
	FMarkdownTextMarshaller();
};


