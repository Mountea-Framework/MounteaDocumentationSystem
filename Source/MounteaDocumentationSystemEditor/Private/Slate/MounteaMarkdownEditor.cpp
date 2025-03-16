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
#include "Policies/CondensedJsonPrintPolicy.h"
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
			position: relative;
		}
		
		.line-numbers {
			position: absolute;
			top: 0;
			left: 0;
			width: 40px;
			height: 100%;
			background-color: #2d2d2d;
			color: #6a6a6a;
			text-align: right;
			padding: 10px 5px 10px 0;
			font-size: 14px;
			line-height: 1.5;
			font-family: 'Courier New', monospace;
			z-index: 10;
			pointer-events: none;
			user-select: none;
		}
		
		.text-editor {
			flex: 1;
			width: 100%;
			padding: 10px 10px 10px 50px;
			background-color: #1e1e1e;
			color: #d4d4d4;
			border: none;
			resize: none;
			outline: none;
			font-size: 14px;
			line-height: 1.5;
			font-family: 'Courier New', monospace;
			tab-size: 4;
		}

		/* Essential code block styling */
		pre, code {
			white-space: pre !important;
			tab-size: 4;
		}
		
		pre {
			padding: 1em;
			margin: .5em 0;
			overflow: auto;
			background: #f5f5f5;
			border: 1px solid #ccc;
		}
		
		code {
			font-family: 'Courier New', Consolas, Monaco, 'Andale Mono', monospace;
		}
	</style>
	<script src="https://cdnjs.cloudflare.com/ajax/libs/marked/9.0.3/marked.min.js"></script>
	<script src="https://cdnjs.cloudflare.com/ajax/libs/prism/1.29.0/prism.min.js"></script>
	<script src="https://cdnjs.cloudflare.com/ajax/libs/prism/1.29.0/components/prism-cpp.min.js"></script>
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
	bNeedsContentRefresh = true;
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
		.OnHtmlGenerated(FOnHtmlGeneratedDelegate::CreateSP(this, &SMounteaMarkdownEditor::HandleHtmlGenerated))
	];
	
	if (WebEditorWidget.IsValid())
	{
		WebEditorWidget->LoadString(dummyURL, TEXT(""));
		bNeedsContentRefresh = true;
	}
}

END_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	static float elapsedTime = 0.0f;
	if (bNeedsContentRefresh)
	{
		elapsedTime += InDeltaTime;
		if (elapsedTime > 2.5f && WebEditorWidget.IsValid() && EditedPage.IsValid())
		{
			SendContentToEditor();
			bNeedsContentRefresh = false;
		}
	}
}

void SMounteaMarkdownEditor::SendContentToEditor()
{
	if (!WebEditorWidget.IsValid() || !EditedPage.IsValid()) return;
	
	FString Content = EditedPage->PageContent.ToString();
	
	Content.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
	Content.ReplaceInline(TEXT("\""), TEXT("\\\""));
	Content.ReplaceInline(TEXT("\r"), TEXT("\\r"));
	Content.ReplaceInline(TEXT("\n"), TEXT("\\n"));
	Content.ReplaceInline(TEXT("\t"), TEXT("\\t"));
	Content.ReplaceInline(TEXT("'"), TEXT("\\'"));
	
	FString JavaScript = FString::Printf(TEXT("if(window.setContent) { window.setContent(\"%s\"); }"), *Content);
	WebEditorWidget->ExecuteJavascript(JavaScript);
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
			SendContentToEditor();
	}
}

void SMounteaMarkdownEditor::HandleChildTextChanged(const FText& NewText)
{
	SetText(NewText);	
	ConvertMarkdownToHTMLTextOnline();
}

void SMounteaMarkdownEditor::HandleContentChanged(const FString& NewContent)
{
	if (!EditedPage.IsValid()) return;
	EditedPage->PageContent = FText::FromString(NewContent);

	ConvertMarkdownToHTMLTextOnline();
}

void SMounteaMarkdownEditor::HandleHtmlGenerated(const FString& HtmlContent)
{
	if (!EditedPage.IsValid()) return;
	
	const FString newHTML = UMounteaDocumentationSystemStatics::RawHTMLToPage(HtmlContent);
	EditedPage->TranslatedPageContent = FText::FromString(newHTML);
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
	text.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
	text.ReplaceInline(TEXT("\""), TEXT("\\\""));
	text.ReplaceInline(TEXT("\r"), TEXT("\\r"));
	text.ReplaceInline(TEXT("\n"), TEXT("\\n"));
	text.ReplaceInline(TEXT("\t"), TEXT("\\t"));
	text.ReplaceInline(TEXT("'"), TEXT("\\'"));
	
	FString JavaScript = R"(
(function() {
	
	if (typeof marked === 'undefined') {
		console.error('MOUNTEA_INFO:Marked library not found!');
		return null;
	}
	
	try {
		// Use DOMPurify if available
		const purify = typeof DOMPurify !== 'undefined' ? DOMPurify.sanitize : (x) => x;
		
		// Create custom renderer
		const renderer = new marked.Renderer();
		
		// Override code block rendering to use Prism
		renderer.code = function(code, language) {
			// Preserve original whitespace and escape HTML properly
			code = code
				.replace(/&/g, '&amp;')
				.replace(/</g, '&lt;')
				.replace(/>/g, '&gt;')
				.replace(/"/g, '&quot;')
				.replace(/'/g, '&#39;');
				
			const langClass = language ? `language-${language}` : '';
			return `<pre><code class="${langClass}">${code}</code></pre>`;
		};
		
		// Configure marked with the custom renderer
		marked.setOptions({
			renderer: renderer,
			gfm: true,
			breaks: false,
			pedantic: false,
			smartLists: true
		});
		
		// Get markdown from the editor
		const editor = document.getElementById('text-editor');
		if (!editor) {
			console.error('MOUNTEA_INFO:Editor element not found!');
			return null;
		}
		
		// Convert and send the HTML
		const markdown = editor.value;
		const html = marked.parse(markdown);
		console.log('MOUNTEA_HTML_GENERATED:' + html);
		return html;
	} catch(e) {
		console.error('MOUNTEA_INFO:Markdown conversion error:', e.message);
		console.error('MOUNTEA_INFO:Error stack:', e.stack);
		return null;
	}
})();
)";
	
	// Execute the JavaScript
	if (WebEditorWidget.IsValid())
	{
		WebEditorWidget->ExecuteJavascript(JavaScript);
	}
	else
	{
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&text);
		Writer->WriteObjectStart();
		Writer->WriteValue(TEXT("text"), EditedPage->PageContent.ToString());
		Writer->WriteValue(TEXT("mode"), TEXT("markdown"));
		Writer->WriteValue(TEXT("X-GitHub-Api-Version"), TEXT("2022-11-28"));
		Writer->WriteObjectEnd();
		Writer->Close();

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(TEXT("https://api.github.com/markdown"));
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetHeader(TEXT("User-Agent"), TEXT("MounteaDocumentationSystem"));
		Request->SetContentAsString(text);

		Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, const FHttpResponsePtr& Response, bool bSuccess)
		{
			if (bSuccess && Response.IsValid() && (Response->GetResponseCode() >= 200 && Response->GetResponseCode() < 300))
			{
				FString HtmlResponse = Response->GetContentAsString();
				const FString newHTML = UMounteaDocumentationSystemStatics::RawHTMLToPage(HtmlResponse);
				EditedPage->TranslatedPageContent = FText::FromString(newHTML);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Markdown conversion failed!/nCode: %d\nMessage: %s"), Response->GetResponseCode(), *Response->GetContentAsString());
				ConvertMarkdownToHTMLText();
			}
		});

		Request->ProcessRequest();
	}
}
