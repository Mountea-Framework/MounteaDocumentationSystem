// All rights reserved Dominik Morse 2024


#include "Style/MounteaDocumentationStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Settings/MounteaDocumentationSystemSettings.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr< FSlateStyleSet > FMounteaDocumentationStyle::StyleInstance = nullptr;

void FMounteaDocumentationStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMounteaDocumentationStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const ISlateStyle& FMounteaDocumentationStyle::Get()
{
	return *StyleInstance;
}

FName FMounteaDocumentationStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MounteaDocumentationStyle"));
	return StyleSetName;
}

void FMounteaDocumentationStyle::ReloadStyles()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		StyleInstance.Reset();
	}
   
	StyleInstance = Create();
	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon12x12(12.0f, 12.0f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> FMounteaDocumentationStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("MounteaDocumentationStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaDocumentationSystem")->GetBaseDir() / TEXT("Resources"));

	UMounteaDocumentationSystemSettings* Settings = GetMutableDefault<UMounteaDocumentationSystemSettings>();
	if (!ensure(Settings))
	{
		return Style;
	}

	Style->Set("Mountea.CodeUnderlineBrush",
		new IMAGE_BRUSH("Textures/UnderlineBrush", Icon16x16)
	);
	const FSlateFontInfo DefaultFont      = FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 15);
	const FSlateFontInfo DefaultMonoFont  = TTF_FONT(TEXT("Fonts/JetBrainsMono-Regular"), 13);
	
	for (const FName& TextStyleName : Settings->TextTypes)
	{
		FTextBlockStyle TextStyle;
		TextStyle.ColorAndOpacity = Settings->FontColor;

		if (TextStyleName.IsEqual("Bold"))
		{
			// Example: Bold text
			TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 15));
		}
		else if (TextStyleName.IsEqual("Italic"))
		{
			TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Italic"), 15));
		}
		else if (TextStyleName.IsEqual("Code") || TextStyleName.IsEqual("CodeBlock"))
		{
			TextStyle.SetFont(DefaultMonoFont);
			TextStyle.SetColorAndOpacity(FLinearColor::Gray);
			TextStyle.SetUnderlineBrush(*Style->GetBrush("Mountea.CodeUnderlineBrush"));
			TextStyle.UnderlineBrush = (*Style->GetBrush("Mountea.CodeUnderlineBrush"));
			TextStyle.SetHighlightShape(*Style->GetBrush("Mountea.CodeUnderlineBrush"));
			TextStyle.HighlightShape = (*Style->GetBrush("Mountea.CodeUnderlineBrush"));
		}
		else if (TextStyleName.IsEqual("Header 1"))
		{
			TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 32));
		}
		else if (TextStyleName.IsEqual("Header 2"))
		{
			TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 28));
		}
		else if (TextStyleName.IsEqual("Header 3"))
		{
			TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 24));
		}
		else if (TextStyleName.IsEqual("Header 4"))
		{
			TextStyle.SetFont(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 20));
		}
		else
		{
			// Catch-all fallback for anything else (Regular, Link, etc.)
			TextStyle.SetFont(DefaultFont);
		}

		// Register in the style set as "RichTextBlock.Mountea.<StyleName>"
		const FString StyleName = FString::Printf(TEXT("RichTextBlock.Mountea.%s"), *TextStyleName.ToString());
		Style->Set(*StyleName, TextStyle);
	}

	return Style;
}


#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT