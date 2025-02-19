// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Text/SMultiLineEditableText.h"

class SMounteaMarkdownTextEditor : public SMultiLineEditableText
{
public:
	SLATE_BEGIN_ARGS(SMounteaMarkdownTextEditor) {}
		SLATE_ARGUMENT(TWeakObjectPtr<class UMounteaDocumentationPage>, EditedPage)
		SLATE_ATTRIBUTE(FSlateFontInfo, EditorFont)
	SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
	FReply HandleTabPress(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent);
	TWeakObjectPtr<UMounteaDocumentationPage> EditedPage;
};

