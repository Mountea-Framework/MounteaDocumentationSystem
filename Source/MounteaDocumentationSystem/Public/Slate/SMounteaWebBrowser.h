// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "SWebBrowser.h"

/**
 * Enhanced WebBrowser widget for Mountea with dynamic URL binding capability
 */
class MOUNTEADOCUMENTATIONSYSTEM_API SMounteaWebBrowser : public SWebBrowser
{
public:
	
	SLATE_BEGIN_ARGS(SMounteaWebBrowser)
		: _InitialURL(TEXT(""))
		, _ShowControls(true)
		, _BrowserFps(30)
		, _ShowAddressBar(true)
		, _ShowErrorMessage(true)
		, _ShowInitialThrobber(true)
		, _SupportsTransparency(false)
		, _ViewportSize(FVector2D(320, 240))
		, _BackgroundColor(255, 255, 255, 255)
		, _OnUrlChanged()
		, _OnBeforePopup()
		, _OnCreateWindow()
		, _OnCloseWindow()
		, _OnBeforeNavigation()
		, _OnLoadUrl()
	{}
		/** Dynamic URL binding */
		SLATE_ATTRIBUTE(FText, URL)
		/** URL that the browser will initially navigate to */
		SLATE_ARGUMENT(FString, InitialURL)
		/** Whether to show standard navigation controls */
		SLATE_ARGUMENT(bool, ShowControls)
		/** Sets browser target fps */
		SLATE_ARGUMENT(int32, BrowserFps)
		/** Whether to show the address bar */
		SLATE_ARGUMENT(bool, ShowAddressBar)
		/** Whether to show error messages */
		SLATE_ARGUMENT(bool, ShowErrorMessage)
		/** Whether to show the initial loading throbber */
		SLATE_ARGUMENT(bool, ShowInitialThrobber)
		/** Should this browser support transparency */
		SLATE_ARGUMENT(bool, SupportsTransparency)
		/** The initial size of the browser viewport */
		SLATE_ARGUMENT(FVector2D, ViewportSize)
		/** Background color of the browser */
		SLATE_ARGUMENT(FColor, BackgroundColor)
		/** Called when the URL changes */
		SLATE_EVENT(FOnTextChanged, OnUrlChanged)
		/** Called before a popup is spawned */
		SLATE_EVENT(FOnBeforePopupDelegate, OnBeforePopup)
		/** Called when a new window is created */
		SLATE_EVENT(FOnCreateWindowDelegate, OnCreateWindow)
		/** Called when closing this window */
		SLATE_EVENT(FOnCloseWindowDelegate, OnCloseWindow)
		/** New event: Called before navigation */
		SLATE_EVENT(FOnTextChanged, OnBeforeNavigation)
		/** New event: Called when a URL is loaded */
		SLATE_EVENT(FOnTextChanged, OnLoadUrl)
		/** New event: Called when a link is clicked */
		SLATE_EVENT(FOnTextChanged, OnLinkClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

protected:

	/** Injects JavaScript to capture link clicks */
	void InjectLinkClickScript();
	
	void HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity);

	/** Called when a link is clicked in the browser */
	void OnLinkClickedInternal(const FString& ClickedURL);

private:

	FOnTextChanged OnLinkClicked;
	
	/** Dynamic URL attribute */
	TAttribute<FText> URLAttribute;
	
	/** Last URL loaded from the attribute */
	FString LastLoadedURL;
};