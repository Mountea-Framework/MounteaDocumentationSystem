// All rights reserved Dominik Morse 2024

#include "Slate/SMounteaWebBrowser.h"

void SMounteaWebBrowser::Construct(const FArguments& InArgs)
{
	URLAttribute = InArgs._URL;
	LastLoadedURL = FString();
	OnLinkClicked = InArgs._OnLinkClicked;
	OnContentChanged = InArgs._OnContentChanged;
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
    // This script preserves the styling while setting up the editor
    FString Script = R"(
console.log('Script injected - checking styles');

// Log all stylesheets and styles for debugging
var styles = document.styleSheets;
console.log('Style sheets found:', styles.length);

for (var i = 0; i < styles.length; i++) {
    try {
        console.log('Style sheet', i, ':', styles[i]);
    } catch (e) {
        console.log('Error accessing stylesheet', i);
    }
}

// Get the editor element
var editor = document.getElementById('text-editor');
if (editor) {
    console.log('Found editor element');
    console.log('Editor classes:', editor.className);
    console.log('Line numbers element:', !!document.getElementById('line-numbers'));
    
    // Setup input handler with debugging
    editor.addEventListener('input', function() {
        console.log('Input event fired');
        console.log('MOUNTEA_CONTENT_CHANGED:' + editor.value);
    });
    
    // Setup content setting function
    window.setContent = function(content) {
        editor.value = content;
        console.log('Content set, length:', content.length);
        
        // Update line numbers if function exists
        if (window.updateLineNumbers) {
            window.updateLineNumbers();
        }
        
        return true;
    };
    
    // Setup line numbering function if it doesn't exist
    if (!window.updateLineNumbers) {
        window.updateLineNumbers = function() {
            var lineNumbers = document.getElementById('line-numbers');
            if (lineNumbers) {
                var lines = editor.value.split('\n');
                var html = '';
                for (var i = 1; i <= lines.length; i++) {
                    html += i + '<br>';
                }
                lineNumbers.innerHTML = html;
                console.log('Line numbers updated:', lines.length);
            }
        };
        
        // Initial line numbers
        window.updateLineNumbers();
        
        // Set up automatic line number updates on input
        editor.addEventListener('input', window.updateLineNumbers);
    }
} else {
    console.log('Editor element not found - DOM may not be fully loaded');
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
	// Add debug logging
	UE_LOG(LogTemp, Log, TEXT("WebBrowser console: %s"), *Message);
	
	const FString LinkPrefix = TEXT("MOUNTEA_LINK_CLICKED:");
	const FString ContentPrefix = TEXT("MOUNTEA_CONTENT_CHANGED:");
	
	if (Message.StartsWith(LinkPrefix))
	{
		const FString URL = Message.RightChop(LinkPrefix.Len());
		OnLinkClicked.Execute(FText::FromString(URL));
	}
	else if (Message.StartsWith(ContentPrefix) && OnContentChanged.IsBound())
	{
		const FString Content = Message.RightChop(ContentPrefix.Len());
		UE_LOG(LogTemp, Warning, TEXT("Content changed detected: %s"), *Content.Left(20));
		OnContentChanged.Execute(Content);
	}
}