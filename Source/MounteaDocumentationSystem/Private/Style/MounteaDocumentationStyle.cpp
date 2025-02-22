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

TSharedRef<class FSlateStyleSet> FMounteaDocumentationStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MounteaDocumentationStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MounteaDocumentationSystem")->GetBaseDir() / TEXT("Resources"));

	const UMounteaDocumentationSystemSettings* Settings = GetDefault<UMounteaDocumentationSystemSettings>();
	
	for (const auto& FontPair : Settings->FontMappings)
	{
		FTextBlockStyle TextStyle;
		TextStyle.SetFont(FontPair.Value.PreviewFont);
       
		const FString StyleName = FString::Printf(TEXT("RichTextBlock.Mountea.%s"), *FontPair.Key.ToString());
		Style->Set(*StyleName, TextStyle);
	}
	
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT