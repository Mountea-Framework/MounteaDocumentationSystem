// All rights reserved Dominik Morse 2024

#include "MounteaMarkdownEditor.h"

#include "HttpModule.h"
#include "MounteaMarkdownTextEditor.h"
#include "Core/MounteaDocumentationPage.h"
#include "Dom/JsonObject.h"
#include "Fonts/FontMeasure.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Settings/MounteaDocumentationSystemEditorSettings.h"
#include "Slate/SMounteaWebBrowser.h"
#include "Statics/MounteaDocumentationSystemStatics.h"

const FString dummyURL =  R"(
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<style>
		html, body {
			height: 100%;
			margin: 0;
			padding: 0;
			overflow: hidden;
			background-color: #1e1e1e;
			font-family: 'Courier New', monospace;
		}
		
		.editor-container {
			display: flex;
			height: 100%;
		}
		
		.line-numbers {
			width: 40px;
			background-color: #2d2d2d;
			color: #6a6a6a;
			text-align: right;
			padding: 10px 5px 10px 0;
			overflow: hidden;
			font-size: 14px;
			line-height: 1.5;
			font-family: 'Courier New', monospace;
		}
		
		.text-editor {
			flex: 1;
			background-color: #1e1e1e;
			color: #d4d4d4;
			padding: 10px;
			border: none;
			resize: none;
			outline: none;
			font-size: 14px;
			line-height: 1.5;
			font-family: 'Courier New', monospace;
			tab-size: 4;
		}
	</style>
</head>
<body>
	<div class="editor-container">
		<div id="line-numbers" class="line-numbers"></div>
		<textarea id="text-editor" class="text-editor" spellcheck="false"></textarea>
	</div>
</body>
</html>
)";

const UMounteaDocumentationSystemEditorSettings* editorSettings = nullptr;

void SMounteaMarkdownEditor::Construct(const FArguments& InArgs)
{
	EditedPage = InArgs._EditedPage;
	UpdateMarkdownEditor();
}

BEGIN_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::UpdateMarkdownEditor()
{
	ChildSlot
	[
		SAssignNew(WebEditorWidget, SMounteaWebBrowser)
		.InitialURL(TEXT("main"))
		.ShowControls(false)
		.ShowAddressBar(false)
		.ShowInitialThrobber(false)
		.OnContentChanged(FOnContentChangedDelegate::CreateSP(this, &SMounteaMarkdownEditor::HandleContentChanged))
	];
	
	if (WebEditorWidget.IsValid())
	{
		WebEditorWidget->LoadString(dummyURL, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("HTML loaded with full styling"));
	}
}

END_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	// Simple delay before setting content
	static float elapsedTime = 0.0f;
	static bool contentSet = false;
    
	if (!contentSet)
	{
		elapsedTime += InDeltaTime;
        
		// Try to set content after 5 seconds
		if (elapsedTime > 1.0f && WebEditorWidget.IsValid() && EditedPage.IsValid())
		{
			FString Content = EditedPage->PageContent.ToString();
			Content.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
			Content.ReplaceInline(TEXT("'"), TEXT("\\'"));
            
			// First verify setContent exists by running a test function
			WebEditorWidget->ExecuteJavascript(TEXT("console.log('setContent exists:', !!window.setContent);"));
            
			// Then try to set content
			FString JavaScript = FString::Printf(TEXT("if(window.setContent) { var result = window.setContent('%s'); console.log('Content set result:', result); }"), *Content);
			WebEditorWidget->ExecuteJavascript(JavaScript);
            
			UE_LOG(LogTemp, Warning, TEXT("Content set request sent, length: %d"), Content.Len());
			contentSet = true;
		}
	}
}

FText SMounteaMarkdownEditor::GetText() const
{
	return EditedPage.IsValid() ? EditedPage->PageContent : FText::GetEmpty();
}

void SMounteaMarkdownEditor::SetText(const FText& NewText)
{
	if (EditedPage.IsValid())
	{
		EditedPage->PageContent = NewText;
		
		if (WebEditorWidget.IsValid())
		{
			WebEditorWidget->Reload();
		}
	}
}

void SMounteaMarkdownEditor::HandleChildTextChanged(const FText& NewText)
{
	SetText(NewText);
	
	ConvertMarkdownToHTMLTextOnline();
}

void SMounteaMarkdownEditor::HandleContentChanged(const FString& NewContent)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *NewContent)
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
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(FString::Printf(TEXT("https://api.markdown.rocks/api/convert?text=%s&dialect=github"), *FGenericPlatformHttp::UrlEncode(text)));
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("User-Agent"), TEXT("MounteaDocumentationSystem"));

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
