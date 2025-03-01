// All rights reserved Dominik Morse 2024

// All rights reserved Dominik Morse 2024

// All rights reserved Dominik Morse 2024

#include "Slate/SMounteaWebBrowser.h"

void SMounteaWebBrowser::Construct(const FArguments& InArgs)
{
	URLAttribute = InArgs._URL;
	LastLoadedURL = FString();
	OnLinkClicked = InArgs._OnLinkClicked;
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
			InjectLinkClickScript();
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
			InjectLinkClickScript();
			bPendingReloadInjection = false;
		}
	}
}

void SMounteaWebBrowser::InjectLinkClickScript()
{
	// First, clear any existing handler
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
	
	// Then inject the new handler
	FString Script = R"(
		window.mounteaLinkHandler = function(event) {
			let target = event.target;
			while (target && target.tagName !== 'A' && target.parentElement) {
				target = target.parentElement;
			}

			if (target && target.tagName === 'A') {
				console.log('MOUNTEA_LINK_CLICKED:' + target.href);
				event.preventDefault();
				event.stopPropagation();
				return false;
			}
		};

		document.addEventListener('click', window.mounteaLinkHandler, true);
		console.log('Mountea link handler injected');
	)";

	ExecuteJavascript(Script);
}

void SMounteaWebBrowser::HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity)
{
	const FString Prefix = TEXT("MOUNTEA_LINK_CLICKED:");
	if (Message.StartsWith(Prefix))
	{
		FString URL = Message.RightChop(Prefix.Len());
		OnLinkClickedInternal(URL);
	}
}

void SMounteaWebBrowser::OnLinkClickedInternal(const FString& ClickedURL)
{
	if (OnLinkClicked.IsBound())
		OnLinkClicked.Execute(FText::FromString(ClickedURL));

	if (ClickedURL.StartsWith("mountea://"))
	{
		// Handle internal protocol links
		FString ResourcePath = ClickedURL.RightChop(10);
		UE_LOG(LogTemp, Error, TEXT("Internal link to resource: %s"), *ResourcePath);
		
		// TODO: Handle the internal resource link
	}
	else if (ClickedURL.StartsWith("http://") || ClickedURL.StartsWith("https://"))
	{
		// External URL - open in system browser
		FPlatformProcess::LaunchURL(*ClickedURL, nullptr, nullptr);
	}
}
