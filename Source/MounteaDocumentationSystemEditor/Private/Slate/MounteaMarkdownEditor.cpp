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

const FString dummyURL = R"(
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/simplemde/latest/simplemde.min.css">
	<script src="https://cdn.jsdelivr.net/simplemde/latest/simplemde.min.js"></script>
	<script src="https://cdn.jsdelivr.net/npm/marked/marked.min.js"></script>
	<style>
		html, body { 
			height: 100%; 
			margin: 0; 
			padding: 0; 
			overflow: hidden; 
			font-family: 'Courier New', monospace;
		}
		
		#editor-container { 
			height: 100%; 
			display: flex;
			flex-direction: column;
		}
		
		.editor-toolbar { 
			flex: 0 0 auto;
			position: sticky;
			top: 0;
			z-index: 100;
			background-color: #f6f6f6;
			border-bottom: 1px solid #ddd;
		}
		
		.CodeMirror, .CodeMirror-scroll, .CodeMirror pre, .editor-preview { 
			font-family: 'Courier New', monospace !important;
			font-size: 14px !important;
		}
		
		.CodeMirror { 
			flex: 1 1 auto;
			height: auto !important;
			min-height: 0 !important;
		}

		.editor-preview {
			font-family: 'Courier New', monospace !important;
		}
		
		.editor-preview code, .editor-preview pre {
			font-family: 'Courier New', monospace !important;
		}

		::-webkit-scrollbar {
			width: 10px;
		}
		::-webkit-scrollbar-track {
			background: #f1f1f1; 
		}
		::-webkit-scrollbar-thumb {
			background: #888; 
		}
		::-webkit-scrollbar-thumb:hover {
			background: #555; 
		}
	</style>
</head>
<body>
	<div id="editor-container">
		<textarea class="text-editor" id="editor"></textarea>
	</div>
	
	<script>
		var simplemde = new SimpleMDE({ 
			element: document.getElementById("editor"),
			spellChecker: false,
			toolbar: false
		});
		
		window.setContent = function(content) {
			console.error("MOUNTEA_INFO:setContent:", content);
			simplemde.value(content);
			return true;
		};
		
		simplemde.codemirror.on("change", function() {
			console.log("MOUNTEA_CONTENT_CHANGED:" + simplemde.value());
		});
		
		window.convertToHTML = function() {
			try {
				var markdown = simplemde.value();
				var renderer = new marked.Renderer();
				renderer.code = function(code, language) {
					const htmlEscapedCode = code
						.replace(/&/g, '&amp;')
						.replace(/</g, '&lt;')
						.replace(/>/g, '&gt;')
						.replace(/"/g, '&quot;')
						.replace(/'/g, '&#39;');
					
					const preservedCode = htmlEscapedCode
						.split('\n')
						.map(line => `<span>${line}</span>`)
						.join('\n');
					
					const langClass = language ? ` class="language-${language}"` : '';
					return `<pre><code${langClass}>${preservedCode}</code></pre>`;
				};
				
				var html = marked.parse(markdown, { renderer: renderer });
				console.log("MOUNTEA_HTML_GENERATED:" + html);
				return html;
			} catch(e) {
				console.error("MOUNTEA_INFO:Error in markdown conversion:", e);
				var html = simplemde.markdown(simplemde.value());
				console.log("MOUNTEA_HTML_GENERATED:" + html);
				return html;
			}
		};
	</script>
</body>
</html>
)";

const UMounteaDocumentationSystemEditorSettings* editorSettings = nullptr;

void SMounteaMarkdownEditor::Construct(const FArguments& InArgs)
{
	EditedPage = InArgs._EditedPage;
	bNeedsContentRefresh = true;
	lastTime = 0.f;
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
		lastTime = 0.f;
	}
}

END_FUNCTION_BUILD_OPTIMIZATION

void SMounteaMarkdownEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bNeedsContentRefresh)
	{
		lastTime += InDeltaTime;
		if (lastTime > 1.f && WebEditorWidget.IsValid() && EditedPage.IsValid())
		{
			SendContentToEditor();
			bNeedsContentRefresh = false;
			lastTime = 0.f;
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
	
	FString JavaScript = R"(
	(function attemptSetContent() {
		if (window.setContent) {
			const success = window.setContent(")" + Content + R"(");
			console.log('MOUNTEA_INFO:Content set result:', success);
			return success;
		} else {
			console.log('MOUNTEA_INFO:setContent not available, retrying in 100ms');
			setTimeout(attemptSetContent, 100);
			return false;
		}
	})();
	)";
	
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
	EditedPage->MarkPackageDirty();

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
	if (!EditedPage.IsValid() || !WebEditorWidget.IsValid()) return;
	
	FString JavaScript = "window.convertToHTML();";
	WebEditorWidget->ExecuteJavascript(JavaScript);
	
	/*
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
		// Create custom renderer that preserves whitespace
		const renderer = new marked.Renderer();
		
		// This is the key fix for code blocks
		renderer.code = function(code, language) {
			// Preserve line breaks and indentation
			const htmlEscapedCode = code
				.replace(/&/g, '&amp;')
				.replace(/</g, '&lt;')
				.replace(/>/g, '&gt;')
				.replace(/"/g, '&quot;')
				.replace(/'/g, '&#39;');
			
			// Make sure each line is properly preserved
			const preservedCode = htmlEscapedCode
				.split('\n')
				.map(line => `<span>${line}</span>`)
				.join('\n');
			
			const langClass = language ? ` class="language-${language}"` : '';
			return `<pre><code${langClass}>${preservedCode}</code></pre>`;
		};
		
		// Configure marked with the custom renderer and options that preserve whitespace
		marked.setOptions({
			renderer: renderer,
			gfm: true,
			breaks: false,
			pedantic: false,
			sanitize: false,
			smartLists: true,
			smartypants: false,
			xhtml: true
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
	*/
}
