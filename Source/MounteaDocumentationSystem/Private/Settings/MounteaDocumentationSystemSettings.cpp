// All rights reserved Dominik Morse 2024


#include "Settings/MounteaDocumentationSystemSettings.h"

UMounteaDocumentationSystemSettings::UMounteaDocumentationSystemSettings()
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Documentation System");

	SetDefaultTextTypes();
}

void UMounteaDocumentationSystemSettings::SetDefaultTextTypes()
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