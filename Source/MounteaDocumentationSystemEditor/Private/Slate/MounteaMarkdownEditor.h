// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SMultiLineEditableText;

class SMounteaMarkdownEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaMarkdownEditor) {}
		SLATE_ARGUMENT(TWeakObjectPtr<class UMounteaDocumentationPage>, EditedPage)
		SLATE_ATTRIBUTE(FSlateFontInfo, EditorFont)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void UpdateMarkdownEditor();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	FText GetText() const;
	void SetText(const FText& NewText);
	FText GetLineNumbers() const;

	FSlateFontInfo GetEditorFont() const;
	FSlateColor GetLineNumberColor() const;

private:
	FReply HandleTabPress(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent);

	TWeakObjectPtr<UMounteaDocumentationPage> EditedPage;
	TSharedPtr<SMultiLineEditableText> EditableTextWidget;
};

