// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "WebBrowser/Public/SWebBrowser.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

DECLARE_DELEGATE_OneParam(FOnLinkClickedDelegate, const FText&);
DECLARE_DELEGATE_OneParam(FOnContentChangedDelegate, const FString&);
DECLARE_DELEGATE_OneParam(FOnHtmlGeneratedDelegate, const FString&);

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
		, _ViewportSize(FVector2D(1280, 720))
		, _BackgroundColor(255, 255, 255, 255)
		, _BrowserFps(60)
	{}
		SLATE_ATTRIBUTE(FText, URL)
		SLATE_ARGUMENT(FString, InitialURL)
		SLATE_ARGUMENT(bool, ShowControls)
		SLATE_ARGUMENT(bool, ShowAddressBar)
		SLATE_ARGUMENT(bool, ShowErrorMessage)
		SLATE_ARGUMENT(bool, ShowInitialThrobber)
		SLATE_ARGUMENT(bool, SupportsTransparency)
		SLATE_ARGUMENT(FVector2D, ViewportSize)
		SLATE_ARGUMENT(FColor, BackgroundColor)
		SLATE_EVENT(FOnTextChanged, OnUrlChanged)
		SLATE_EVENT(FOnBeforePopupDelegate, OnBeforePopup)
		SLATE_EVENT(FOnCreateWindowDelegate, OnCreateWindow)
		SLATE_EVENT(FOnCloseWindowDelegate, OnCloseWindow)
		SLATE_EVENT(FOnLoadUrl, OnLoadUrl)
		SLATE_ARGUMENT(int, BrowserFps)
		SLATE_EVENT(FOnLinkClickedDelegate, OnLinkClicked)
		SLATE_EVENT(FOnContentChangedDelegate, OnContentChanged)
		SLATE_EVENT(FOnHtmlGeneratedDelegate, OnHtmlGenerated)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void ClearLinkHandlerScript();
	void CreateLinkHandlerScript();
	void CreateInputConsumeScript();

	void InjectScripts();
	void HandleConsoleMessage(const FString& Message, const FString& Source, int32 Line, EWebBrowserConsoleLogSeverity Severity);

private:
	TAttribute<FText> URLAttribute;
	FString LastLoadedURL;
	
	bool bInitialScriptInjected = false;
	bool bPendingReloadInjection = false;
	float TimeSinceConstruction = 0;
	float TimeSinceReload = 0;

	FOnLinkClickedDelegate OnLinkClicked;
	FOnContentChangedDelegate OnContentChanged;
	FOnHtmlGeneratedDelegate OnHtmlGenerated;
};