// All rights reserved Dominik Morse 2024


#include "Settings/MounteaDocumentationSystemEditorSettings.h"

#include "Engine/Font.h"
#include "Settings/MounteaDocumentationSystemSettings.h"

UMounteaDocumentationSystemEditorSettings::UMounteaDocumentationSystemEditorSettings() : Size(15)
{
	CategoryName = TEXT("Mountea Framework");
	SectionName = TEXT("Mountea Documentation System (Editor)");
}

FSlateFontInfo UMounteaDocumentationSystemEditorSettings::GetEditorFont() const
{
	if (!EditorFont.FontObject)
		return  FCoreStyle::GetDefaultFontStyle("Mono", 12);
	
	return EditorFont;
}

TArray<FName> UMounteaDocumentationSystemEditorSettings::GetFontfaces() const
{
	if (FontFamily.IsNull())
		return TArray<FName>{"Regular"};

	TArray<FName> returnValues;

	for(const FTypefaceEntry& TypefaceEntry : FontFamily.Get()->CompositeFont.DefaultTypeface.Fonts)
	{
		returnValues.Add(TypefaceEntry.Name);
	}

	returnValues.Sort([](const FName& One, const FName& Two) -> bool
	{
		return One.ToString() < Two.ToString();
	});
	
	return returnValues;
}

FSlateFontInfo UMounteaDocumentationSystemEditorSettings::CreateSlateFont() const
{
	if (FontFamily.IsNull() || Typeface.IsNone() || !Typeface.IsValid())
		return FSlateFontInfo();

	return FSlateFontInfo(FontFamily.Get(), Size, Typeface);
}

void UMounteaDocumentationSystemEditorSettings::PostEditChangeProperty(
	struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaDocumentationSystemEditorSettings, FontFamily) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaDocumentationSystemEditorSettings, Typeface) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UMounteaDocumentationSystemEditorSettings, Size))
	{
		EditorFont = 	CreateSlateFont();
	}
}

