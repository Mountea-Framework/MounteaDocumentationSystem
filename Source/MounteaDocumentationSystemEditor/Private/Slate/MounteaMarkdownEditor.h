// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SMounteaMarkdownTextEditor;

class SMounteaMarkdownEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMounteaMarkdownEditor) {}
		SLATE_ARGUMENT(TWeakObjectPtr<class UMounteaDocumentationPage>, EditedPage)
		SLATE_ATTRIBUTE(FSlateFontInfo, EditorFont)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Call this whenever the editor geometry or text changes to update line numbers, etc. */
	void UpdateMarkdownEditor();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	FText GetText() const;
	void SetText(const FText& NewText);
	FText GetLineNumbers() const;

	FSlateFontInfo GetEditorFont() const;
	FSlateColor GetLineNumberColor() const;
	int32 CalculateManualWrappedLineCount() const;

private:
	TWeakObjectPtr<class UMounteaDocumentationPage> EditedPage;
	TSharedPtr<SMounteaMarkdownTextEditor> EditableTextWidget;
};


