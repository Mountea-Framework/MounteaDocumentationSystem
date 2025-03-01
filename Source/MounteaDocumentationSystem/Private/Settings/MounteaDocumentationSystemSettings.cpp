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
	SetDefaultCSS();
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

void UMounteaDocumentationSystemSettings::SetDefaultCSS()
{
	DisplayCSS = R"(
	html, body {
		height: 100%;
		margin: 0;
		padding: 0;
		box-sizing: border-box;
	}

	body {
		font-family: Arial, sans-serif;
		line-height: 1.6;
		color: #333;
		max-width: 100%;
		overflow: hidden; /* Prevent scrollbars */
		display: flex;
		flex-direction: column;
	}

	main {
		flex: 1;
		overflow-y: auto; /* Allow vertical scrolling inside the main content if needed */
	}

	h1, h2, h3, h4, h5, h6 {
		color: #205081;
		margin-top: 1.5em;
		margin-bottom: 0.5em;
	}

	code {
		background-color: #f5f5f5;
		padding: 2px 4px;
		border-radius: 3px;
		font-family: monospace;
	}

	a {
		color: #3572b0;
		text-decoration: none;
	}

	a:hover {
		text-decoration: underline;
	}

	blockquote {
		border-left: 4px solid #ddd;
		padding-left: 15px;
		color: #555;
	}

	img {
		max-width: 100%;
	}

	pre {
		background-color: #f5f5f5;
		padding: 10px;
		border-radius: 3px;
		overflow-x: auto;
		white-space: pre-wrap;
		word-wrap: break-word;
	}

	pre code {
		background-color: transparent;
		padding: 0;
		white-space: inherit;
		display: block;
	}

	ul, ol {
		padding-left: 20px;
	}

	.mountea-markdown-table {
	  border-collapse: collapse;
	  width: 100%;
	  margin: 1em 0;
	}

	.mountea-markdown-table th, 
	.mountea-markdown-table td {
	  border: 1px solid #ddd;
	  padding: 8px;
	  text-align: left;
	}

	.mountea-markdown-table th {
	  background-color: #f2f2f2;
	  font-weight: bold;
	}

	.mountea-markdown-table tr:nth-child(even) {
	  background-color: #f9f9f9;
	}
	)";
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