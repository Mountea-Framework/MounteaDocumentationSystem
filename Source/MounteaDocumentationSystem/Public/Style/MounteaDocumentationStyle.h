// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class UMounteaDocumentationSystemSettings;

class MOUNTEADOCUMENTATIONSYSTEM_API  FMounteaDocumentationStyle : public FAppStyle
{
public:
	static void Initialize();

	static void Shutdown();

	static const FSlateBrush * GetBrush(FName PropertyName, const ANSICHAR* Specifier = NULL)
	{
		return StyleInstance->GetBrush(PropertyName, Specifier);
	};

	/** @return The Slate style set for the Actor Interaction Plugin Help Button */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

	static void ReloadStyles();

private:

	static TSharedRef< class FSlateStyleSet > Create();
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};
