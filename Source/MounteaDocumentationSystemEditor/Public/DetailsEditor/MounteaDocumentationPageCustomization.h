// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class FMounteaDocumentationPageCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	FText GetMarkdownText() const;
	FText GetRenderedMarkdownText() const;
	void OnMarkdownTextChanged(const FText& NewText);

	TWeakObjectPtr<class UMounteaDocumentationPage> TargetPage;
};
