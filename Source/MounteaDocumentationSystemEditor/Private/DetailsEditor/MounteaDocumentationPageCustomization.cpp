// All rights reserved Dominik Morse 2024

#include "DetailsEditor/MounteaDocumentationPageCustomization.h"

#include "Core/MounteaDocumentationPage.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"

#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FMounteaDocumentationPageCustomization"

TSharedRef<IDetailCustomization> FMounteaDocumentationPageCustomization::MakeInstance()
{
	return MakeShareable(new FMounteaDocumentationPageCustomization);
}

void FMounteaDocumentationPageCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	DetailBuilder.GetObjectsBeingCustomized(SelectedObjects);

	if (SelectedObjects.Num() == 1)
	{
		TargetPage = Cast<UMounteaDocumentationPage>(SelectedObjects[0].Get());
	}

	IDetailCategoryBuilder& MarkdownCategory = DetailBuilder.EditCategory("Markdown");

	MarkdownCategory.AddCustomRow(LOCTEXT("MarkdownEditor", "Markdown Editor"))
	[
		SNew(SHorizontalBox)

		// Markdown Input
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(SMultiLineEditableText)
			.Text(this, &FMounteaDocumentationPageCustomization::GetMarkdownText)
			.OnTextChanged(this, &FMounteaDocumentationPageCustomization::OnMarkdownTextChanged)
		]

		// Rendered Markdown Preview
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(STextBlock)
			.AutoWrapText(true)
			.Text(this, &FMounteaDocumentationPageCustomization::GetRenderedMarkdownText)
		]
	];
	
	MarkdownCategory.SetSortOrder(32);
}

FText FMounteaDocumentationPageCustomization::GetMarkdownText() const
{
	return TargetPage.IsValid() ? TargetPage->PageContent : FText::GetEmpty();
}

FText FMounteaDocumentationPageCustomization::GetRenderedMarkdownText() const
{
	// Placeholder - integrate Markdown parser here
	return TargetPage.IsValid() ? TargetPage->RichTextPageContent : FText::GetEmpty();
}

void FMounteaDocumentationPageCustomization::OnMarkdownTextChanged(const FText& NewText)
{
	if (TargetPage.IsValid())
	{
		TargetPage->PageContent = NewText;
		// Process Markdown and update preview
		TargetPage->RichTextPageContent = FText::FromString(TEXT("[Rendered Markdown]"));
	}
}

#undef LOCTEXT_NAMESPACE
