// All rights reserved Dominik Morse 2024

#include "Slate/SMounteaWebBrowser.h"

void SMounteaWebBrowser::Construct(const FArguments& InArgs)
{
	URLAttribute = InArgs._URL;
	LastLoadedURL = FString();
	
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
	
	InjectLinkClickScript();
}

void SMounteaWebBrowser::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SWebBrowser::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	if (URLAttribute.IsSet())
	{
		FString CurrentAttributeURL = URLAttribute.Get().ToString();
		if (CurrentAttributeURL != LastLoadedURL && !CurrentAttributeURL.IsEmpty())
		{
			LastLoadedURL = CurrentAttributeURL;
			LoadString(CurrentAttributeURL, TEXT(""));
			Reload();
			//InjectLinkClickScript();
		}
	}
}


void SMounteaWebBrowser::InjectLinkClickScript()
{
	FString Script = R"(
        document.addEventListener('click', function(event) {
            // Find link element (handles clicking on child elements)
            let target = event.target;
            while (target && target.tagName !== 'A' && target.parentElement) {
                target = target.parentElement;
            }
            
            if (target && target.tagName === 'A') {
                console.log('Link clicked: ' + target.href);
                // Call the bound UObject method
                ue.mounteabrowser.onlinkclickedfromjs(target.href);
            }
        }, true);
        
        console.log('Link tracking bound to ue.mounteabrowser');
    )";

	ExecuteJavascript(Script);
}

void SMounteaWebBrowser::HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message)
}

void SMounteaWebBrowser::OnLinkClickedInternal(const FString& ClickedURL)
{
	OnLinkClicked.Execute(FText::FromString(ClickedURL));
}
