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
	{}
		/** Dynamic URL binding */
		SLATE_ATTRIBUTE(FText, URL)
		/** URL that the browser will initially navigate to */
		SLATE_ARGUMENT(FString, InitialURL)
		/** Whether to show standard navigation controls */
		SLATE_ARGUMENT(bool, ShowControls)
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
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	/** Dynamic URL attribute */
	TAttribute<FText> URLAttribute;
	
	/** Last URL loaded from the attribute */
	FString LastLoadedURL;
};