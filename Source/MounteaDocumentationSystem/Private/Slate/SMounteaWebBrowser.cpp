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
	);
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
		}
	}
}