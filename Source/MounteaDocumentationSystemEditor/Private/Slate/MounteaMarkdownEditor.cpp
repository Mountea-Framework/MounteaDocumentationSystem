// All rights reserved Dominik Morse 2024

#include "MounteaMarkdownEditor.h"

#include "HttpModule.h"
#include "MounteaMarkdownTextEditor.h"
#include "Core/MounteaDocumentationPage.h"
#include "Dom/JsonObject.h"
#include "Fonts/FontMeasure.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Settings/MounteaDocumentationSystemEditorSettings.h"
#include "Statics/MounteaDocumentationSystemStatics.h"

const UMounteaDocumentationSystemEditorSettings* editorSettings = nullptr;

void SMounteaMarkdownEditor::Construct(const FArguments& InArgs)
{
	EditedPage = InArgs._EditedPage;
	UpdateMarkdownEditor();
	editorSettings = GetDefault<UMounteaDocumentationSystemEditorSettings>();
}

BEGIN_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::UpdateMarkdownEditor()
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		
		// Line Numbers Column
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			.Padding(10)
			.BorderBackgroundColor(FLinearColor::Transparent)
			[
				SNew(STextBlock)
				.Text(this, &SMounteaMarkdownEditor::GetLineNumbers)
				.Justification(ETextJustify::Right)
				.Font(GetEditorFont())
				.ColorAndOpacity(this, &SMounteaMarkdownEditor::GetLineNumberColor)
			]
		]

		// Markdown Input (Editor)
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(SBorder)
			.Padding(10)
			.BorderBackgroundColor(FLinearColor::Transparent)
			[
				SAssignNew(EditableTextWidget, SMounteaMarkdownTextEditor)
				.EditedPage(EditedPage)
				.EditorFont(this, &SMounteaMarkdownEditor::GetEditorFont)
				.OnChildTextChanged(this, &SMounteaMarkdownEditor::HandleChildTextChanged)
			]
		]
	];
}

END_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime,
	const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (EditableTextWidget.IsValid())
	{
		EditableTextWidget->UpdateEditorFont(GetEditorFont());
		const float WrapWidth = AllottedGeometry.GetLocalSize().X - 40.0f;
		EditableTextWidget->SetWrapTextAt(WrapWidth);
		
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}	
}

FText SMounteaMarkdownEditor::GetText() const
{
	return EditedPage.IsValid() ? EditedPage->PageContent : FText::GetEmpty();
}

void SMounteaMarkdownEditor::SetText(const FText& NewText)
{
	if (EditedPage.IsValid())
		EditedPage->PageContent = NewText;
}

FSlateFontInfo SMounteaMarkdownEditor::GetEditorFont() const
{
	if (!IsValid(editorSettings))
		editorSettings = GetMutableDefault<UMounteaDocumentationSystemEditorSettings>();
	return editorSettings->GetEditorFont();
}

FSlateColor SMounteaMarkdownEditor::GetLineNumberColor() const
{
	return FLinearColor(1.f, 1.f, 1.f ,0.2f);
}

int32 SMounteaMarkdownEditor::CalculateManualWrappedLineCount() const
{
	if (!EditedPage.IsValid() || !EditableTextWidget.IsValid())
		return 0;

	float CurrentWidth = EditableTextWidget->GetCachedGeometry().GetLocalSize().X;
	if (CurrentWidth <= 0.f)
		CurrentWidth = 996.f;

	const float BaselineWidth = 996.f;
	const float Ratio = CurrentWidth / BaselineWidth;
	const int32 ScaledCharactersPerLine = FMath::Max(1, FMath::CeilToInt(editorSettings->EstimatedWordPerLine * Ratio));

	const FString FullText = EditedPage->PageContent.ToString();
	TArray<FString> Lines;
	FullText.ParseIntoArray(Lines, TEXT("\n"), false);

	int32 TotalLines = 0;
	for (const FString& Line : Lines)
	{
		const FString Trimmed = Line.TrimStartAndEnd();
		if (Trimmed.IsEmpty())
		{
			++TotalLines;
		}
		else
		{
			int32 ChunkCount = Trimmed.Len() / ScaledCharactersPerLine;
			if (Trimmed.Len() % ScaledCharactersPerLine != 0)
				++ChunkCount;
			TotalLines += ChunkCount;
		}
	}

	return TotalLines;
}

FText SMounteaMarkdownEditor::GetLineNumbers() const
{
	if (!EditableTextWidget.IsValid()) return FText::GetEmpty();

	int32 totalLines = CalculateManualWrappedLineCount();
	FString lineNumbers;
	for (int32 i = 1; i <= totalLines; i++)
	{
		lineNumbers += FString::Printf(TEXT("%d\n"), i);
	}
	return FText::FromString(lineNumbers);
}

void SMounteaMarkdownEditor::HandleChildTextChanged(const FText& NewText)
{
	SetText(NewText);
	
	//ConvertMarkdownToHTMLText();
	ConvertMarkdownToHTMLTextOnline();
}

void SMounteaMarkdownEditor::ConvertMarkdownToRichText() const
{
	if (!EditedPage.IsValid()) return;

	FString text = EditedPage->PageContent.ToString();

	const FString newRichText = UMounteaDocumentationSystemStatics::ConvertMarkdownToRichText(text);

	EditedPage->TranslatedPageContent = FText::FromString(newRichText);
}

void SMounteaMarkdownEditor::ConvertMarkdownToHTMLText() const
{
	if (!EditedPage.IsValid()) return;

	FString text = EditedPage->PageContent.ToString();

	const FString newRawHTML = UMounteaDocumentationSystemStatics::ConvertMarkdownToHTML(text);
	const FString newHTML = UMounteaDocumentationSystemStatics::RawHTMLToPage(newRawHTML);

	EditedPage->TranslatedPageContent = FText::FromString(newHTML);
}

void SMounteaMarkdownEditor::ConvertMarkdownToHTMLTextOnline() const
{
	if (!EditedPage.IsValid()) return;

	FString text = EditedPage->PageContent.ToString();
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("text"), text);
	JsonObject->SetStringField(TEXT("mode"), TEXT("markdown"));

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("https://api.github.com/markdown"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("User-Agent"), TEXT("MounteaDocumentationSystem"));
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, const FHttpResponsePtr& Response, bool bSuccess)
	{
		if (bSuccess && Response.IsValid())
		{
			FString HtmlResponse = Response->GetContentAsString();
			const FString newHTML = UMounteaDocumentationSystemStatics::RawHTMLToPage(HtmlResponse);
			EditedPage->TranslatedPageContent = FText::FromString(newHTML);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Markdown conversion failed!"));
			ConvertMarkdownToHTMLText();
		}
	});

	Request->ProcessRequest();
}
