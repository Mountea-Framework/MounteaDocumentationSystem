// All rights reserved Dominik Morse 2024


#include "MounteaMarkdownTextEditor.h"
#include "Core/MounteaDocumentationPage.h"
#include "Text/MounteaMarkdownTextMarshaller.h"

void SMounteaMarkdownTextEditor::Construct(const FArguments& InArgs)
{
	EditedPage = InArgs._EditedPage;

	MarkdownMarshaller = MakeShared<FMarkdownTextMarshaller>();

	SMultiLineEditableText::Construct(
		SMultiLineEditableText::FArguments()
		.Text_Lambda([this]() -> FText { return EditedPage.IsValid() ? EditedPage->PageContent : FText::GetEmpty(); })
		.OnTextChanged_Lambda([this](const FText& NewText)
		{
			if (EditedPage.IsValid())
			{
				EditedPage->PageContent = NewText;
			}
		})
		.Font(InArgs._EditorFont)
		.AutoWrapText(true)
		.OnKeyDownHandler(this, &SMounteaMarkdownTextEditor::HandleTabPress)
		.Marshaller(MarkdownMarshaller)
	);
}

void SMounteaMarkdownTextEditor::UpdateEditorFont(const FSlateFontInfo& NewFont)
{
	SetFont(NewFont);
}

FReply SMounteaMarkdownTextEditor::HandleTabPress(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	if (KeyEvent.GetKey() == EKeys::Tab)
	{
		InsertTextAtCursor(TEXT("\t"));
		return FReply::Handled();
	}
	return FReply::Unhandled();
}
