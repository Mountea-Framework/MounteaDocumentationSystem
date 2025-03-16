// All rights reserved Dominik Morse 2024

#include "Slate/SMounteaWebBrowser.h"

void SMounteaWebBrowser::Construct(const FArguments& InArgs)
{
	URLAttribute = InArgs._URL;
	LastLoadedURL = FString();
	OnLinkClicked = InArgs._OnLinkClicked;
	OnContentChanged = InArgs._OnContentChanged;
	OnHtmlGenerated = InArgs._OnHtmlGenerated;
	bInitialScriptInjected = false;
	bPendingReloadInjection = false;
	TimeSinceConstruction = 0.0;
	
	SWebBrowser::Construct(
		SWebBrowser::FArguments()
		.InitialURL(InArgs._InitialURL)
		.ShowControls(InArgs._ShowControls)
		.ShowAddressBar(InArgs._ShowAddressBar)
		.ShowErrorMessage(InArgs._ShowErrorMessage)
		.ShowInitialThrobber(InArgs._ShowInitialThrobber)
		.SupportsTransparency(InArgs._SupportsTransparency)
		.ViewportSize(InArgs._ViewportSize)
		.BackgroundColor(InArgs._BackgroundColor)
		.OnUrlChanged(InArgs._OnUrlChanged)
		.OnBeforePopup(InArgs._OnBeforePopup)
		.OnCreateWindow(InArgs._OnCreateWindow)
		.OnCloseWindow(InArgs._OnCloseWindow)
		.BrowserFrameRate(InArgs._BrowserFps)
		.OnConsoleMessage(FOnConsoleMessageDelegate::CreateSP(this, &SMounteaWebBrowser::HandleConsoleMessage))
	);
}

void SMounteaWebBrowser::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SWebBrowser::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	// Handle initial script injection after 2 seconds
	if (!bInitialScriptInjected)
	{
		TimeSinceConstruction += InDeltaTime;
		if (TimeSinceConstruction >= 2.0)
		{
			InjectScripts();
			bInitialScriptInjected = true;
		}
	}
	
	// Handle URL changes
	if (URLAttribute.IsSet())
	{
		FString CurrentAttributeURL = URLAttribute.Get().ToString();
		if (CurrentAttributeURL != LastLoadedURL && !CurrentAttributeURL.IsEmpty())
		{
			LastLoadedURL = CurrentAttributeURL;
			LoadString(CurrentAttributeURL, TEXT(""));
			bPendingReloadInjection = true;
			TimeSinceReload = 0.0;
			Reload();
		}
	}
	
	// Handle script injection after reload (with slight delay to ensure DOM is loaded)
	if (bPendingReloadInjection)
	{
		TimeSinceReload += InDeltaTime;
		if (TimeSinceReload >= 1.0)
		{
			InjectScripts();
			bPendingReloadInjection = false;
		}
	}
}

void SMounteaWebBrowser::ClearLinkHandlerScript()
{
	FString ClearScript = R"(
		try {
			if (window.mounteaLinkHandler) {
				document.removeEventListener('click', window.mounteaLinkHandler, true);
				delete window.mounteaLinkHandler;
				console.log('Previous link handler removed');
			}
		} catch (e) {
			console.log('Error clearing previous handler:', e);
		}
	)";
	
	ExecuteJavascript(ClearScript);
}

void SMounteaWebBrowser::CreateLinkHandlerScript()
{
	FString LinkScript = R"(
		try {
			window.mounteaLinkHandler = function(event) {
				let target = event.target;
				while (target && target.tagName !== 'A' && target.parentElement) {
					target = target.parentElement;
				}

				if (target && target.tagName === 'A') {
					console.log('MOUNTEA_LINK_CLICKED:' + target.href);
					event.preventDefault();
					event.stopPropagation();
				}
			};

			document.addEventListener('click', window.mounteaLinkHandler, true);
			console.log('Mountea link handler injected');
		} catch (e) {
			console.log('Error in link handler:', e);
		}
	)";

	ExecuteJavascript(LinkScript);
}

void SMounteaWebBrowser::CreateInputConsumeScript()
{
	FString Script = R"(
// Core editor functionality
const editor = document.getElementById('text-editor');
const lineNumbers = document.getElementById('line-numbers');

// Line numbering function
function updateLineNumbers() {
	if (!editor || !lineNumbers) return;
	
	const lines = editor.value.split('\n');
	let lineNumbersHTML = '';
	for (let i = 1; i <= lines.length; i++) {
		lineNumbersHTML += i + '<br>';
	}
	lineNumbers.innerHTML = lineNumbersHTML;
	
	// Position line numbers according to editor's scroll position
	lineNumbers.style.transform = `translateY(-${editor.scrollTop}px)`;
	console.log('MOUNTEA_INFO:Line numbers updated:', lines.length);
}

// Setup global content setting function
window.setContent = function(content) {
	if (!editor) {
		console.error('MOUNTEA_INFO:Editor element not found');
		return false;
	}
	
	editor.value = content;
	updateLineNumbers();
	return true;
};

// Setup event listeners if editor exists
if (editor) {	
	// Scroll event for line numbers
	editor.addEventListener('scroll', function() {
		if (lineNumbers) {
			lineNumbers.style.transform = `translateY(-${editor.scrollTop}px)`;
		}
	});
	
	// Input event for content changes and line numbers
	editor.addEventListener('input', function() {
		updateLineNumbers();
		console.log('MOUNTEA_CONTENT_CHANGED:' + editor.value);
	});
	
	// Initialize line numbers
	updateLineNumbers();
} else {
	console.error('MOUNTEA_INFO:Editor element not found - DOM may not be fully loaded');
}
)";

	ExecuteJavascript(Script);
}

void SMounteaWebBrowser::InjectScripts()
{
	// Clear any existing handlers
	ClearLinkHandlerScript();
	
	// Link handler
	CreateLinkHandlerScript();
	
	// Editor functionality
	CreateInputConsumeScript();
}

void SMounteaWebBrowser::HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity)
{
	const FString LinkPrefix = TEXT("MOUNTEA_LINK_CLICKED:");
	const FString ContentPrefix = TEXT("MOUNTEA_CONTENT_CHANGED:");
	const FString InfoPrefix = TEXT("MOUNTEA_INFO:");
	const FString HtmlPrefix = TEXT("MOUNTEA_HTML_GENERATED:");
	
	if (Message.StartsWith(LinkPrefix) && OnLinkClicked.IsBound())
	{
		const FString URL = Message.RightChop(LinkPrefix.Len());
		OnLinkClicked.Execute(FText::FromString(URL));
	}
	else if (Message.StartsWith(ContentPrefix) && OnContentChanged.IsBound())
	{
		const FString Content = Message.RightChop(ContentPrefix.Len());
		OnContentChanged.Execute(Content);
	}
	else if (Message.StartsWith(HtmlPrefix) && OnHtmlGenerated.IsBound())
	{
		const FString HTML = Message.RightChop(HtmlPrefix.Len());
		OnHtmlGenerated.Execute(HTML);
	}
	else if (Message.StartsWith(InfoPrefix))
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message)
}