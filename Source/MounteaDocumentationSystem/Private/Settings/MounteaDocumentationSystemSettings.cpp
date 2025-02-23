// All rights reserved Dominik Morse 2024


#include "Settings/MounteaDocumentationSystemSettings.h"
#include "Engine/Font.h"
#include "Style/MounteaDocumentationStyle.h"

FSlateFontInfo FDocumentationFontMappings::ToSlateFontInto() const
{
	return FSlateFontInfo(FontFamily.LoadSynchronous(), Size, Typeface);
}

UMounteaDocumentationSystemSettings::UMounteaDocumentationSystemSettings()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Documentation System");

	SetDefaultTextTypes();
	RefreshPreviewFonts();
}

void UMounteaDocumentationSystemSettings::SetDefaultTextTypes()
{
	TSet<FName> returnValue;
		returnValue.Add(TEXT("Header 1"));
		returnValue.Add(TEXT("Header 2"));
		returnValue.Add(TEXT("Header 3"));
		returnValue.Add(TEXT("Header 4"));
		returnValue.Add(TEXT("Code"));
		returnValue.Add(TEXT("CodeBlock"));
		returnValue.Add(TEXT("Regular"));
		returnValue.Add(TEXT("Bold"));
		returnValue.Add(TEXT("Italic"));
		returnValue.Add(TEXT("Link"));

	TextTypes.Append(returnValue);

	OnDocumentationPreviewFontChanged.Broadcast();
}

FSlateFontInfo UMounteaDocumentationSystemSettings::GetFont(const FName& Type) const
{
	if (!FontMappings.Contains(Type))
		return  FCoreStyle::GetDefaultFontStyle("Regular", 14);

	auto fontConfig = FontMappings.Find(Type);
	if (!fontConfig->PreviewFont.FontObject)
		return  FCoreStyle::GetDefaultFontStyle("Regular", 12);
	
	return fontConfig->PreviewFont;
}

TArray<FName> UMounteaDocumentationSystemSettings::GetTextTypes() const
{
	TSet<FName> returnValue;
		returnValue.Add(TEXT("Header 1"));
		returnValue.Add(TEXT("Header 2"));
		returnValue.Add(TEXT("Header 3"));
		returnValue.Add(TEXT("Header 4"));
		returnValue.Add(TEXT("Code"));
		returnValue.Add(TEXT("Regular"));
		returnValue.Add(TEXT("Bold"));
		returnValue.Add(TEXT("Italic"));
		returnValue.Add(TEXT("Link"));

	returnValue.Append(TextTypes);
	return returnValue.Array();
}

void UMounteaDocumentationSystemSettings::RefreshPreviewFonts()
{
	for (auto& Pair : FontMappings)
	{
		FDocumentationFontMappings& Mapping = Pair.Value;
		
		if (Mapping.FontFamily.LoadSynchronous())
			Mapping.PreviewFont = GetFont(Pair.Key);
		else
			Mapping.PreviewFont = FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), Mapping.Size);
	}
}

#if WITH_EDITOR
void UMounteaDocumentationSystemSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
   
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UMounteaDocumentationSystemSettings, FontMappings))
	{
		RefreshPreviewFonts();
		FMounteaDocumentationStyle::ReloadStyles();
		OnDocumentationPreviewFontChanged.Broadcast();
	}
}
#endif