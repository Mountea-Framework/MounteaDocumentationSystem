// All rights reserved Dominik Morse 2024


#include "DetailsEditor/DocumentationFontMappingsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Engine/Font.h"
#include "PropertyCustomizationHelpers.h"
#include "Settings/MounteaDocumentationSystemSettings.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "DocumentationFontMappings"

TSharedRef<IPropertyTypeCustomization> FDocumentationFontMappingsCustomization::MakeInstance()
{
	return MakeShareable(new FDocumentationFontMappingsCustomization());
}

void FDocumentationFontMappingsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = InStructPropertyHandle;

	FontFamilyProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDocumentationFontMappings, FontFamily));
	TypefaceProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDocumentationFontMappings, Typeface));
	SizeProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDocumentationFontMappings, Size));

	HeaderRow
		.NameContent()
		[
			InStructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(0)
		.MaxDesiredWidth(0)
		[
			InStructPropertyHandle->CreatePropertyValueWidget()
		];
}

void FDocumentationFontMappingsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Font Family property
	IDetailPropertyRow& FontFamilyRow = StructBuilder.AddProperty(FontFamilyProperty.ToSharedRef());
	FontFamilyRow.CustomWidget()
		.NameContent()
		[
			FontFamilyProperty->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(200.f)
		.MaxDesiredWidth(300.f)
		[
			SNew(SObjectPropertyEntryBox)
			.PropertyHandle(FontFamilyProperty)
			.AllowedClass(UFont::StaticClass())
			.OnShouldFilterAsset_Static(&FDocumentationFontMappingsCustomization::OnFilterFontAsset)
			.OnObjectChanged(this, &FDocumentationFontMappingsCustomization::OnFontChanged)
			.DisplayUseSelected(true)
			.DisplayBrowse(true)
		];

	// Typeface property
	IDetailPropertyRow& TypefaceRow = StructBuilder.AddProperty(TypefaceProperty.ToSharedRef());
	TypefaceRow.CustomWidget()
		.NameContent()
		[
			TypefaceProperty->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SAssignNew(FontEntryCombo, SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&FontEntryComboData)
			.IsEnabled(this, &FDocumentationFontMappingsCustomization::IsFontEntryComboEnabled)
			.OnComboBoxOpening(this, &FDocumentationFontMappingsCustomization::OnFontEntryComboOpening)
			.OnSelectionChanged(this, &FDocumentationFontMappingsCustomization::OnFontEntrySelectionChanged)
			.OnGenerateWidget(this, &FDocumentationFontMappingsCustomization::MakeFontEntryWidget)
			[
				SNew(STextBlock)
				.Text(this, &FDocumentationFontMappingsCustomization::GetFontEntryComboText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];

	// Size property
	StructBuilder.AddProperty(SizeProperty.ToSharedRef());
}

bool FDocumentationFontMappingsCustomization::OnFilterFontAsset(const FAssetData& InAssetData)
{
	// Filter out font assets that aren't valid to use with Slate/UMG
	const UFont* Font = Cast<const UFont>(InAssetData.GetAsset());
	return Font && Font->FontCacheType != EFontCacheType::Runtime;
}

void FDocumentationFontMappingsCustomization::OnFontChanged(const FAssetData& InAssetData)
{
	const UFont* const FontAsset = Cast<const UFont>(InAssetData.GetAsset());
	const FName FirstFontName = (FontAsset && FontAsset->CompositeFont.DefaultTypeface.Fonts.Num()) 
		? FontAsset->CompositeFont.DefaultTypeface.Fonts[0].Name 
		: NAME_None;

	TypefaceProperty->SetValue(FirstFontName);
}

bool FDocumentationFontMappingsCustomization::IsFontEntryComboEnabled() const
{
	UObject* FontObject = nullptr;
	FontFamilyProperty->GetValue(FontObject);
	return FontObject != nullptr;
}

void FDocumentationFontMappingsCustomization::OnFontEntryComboOpening()
{
	FontEntryComboData.Empty();

	UObject* FontObjectPtr = nullptr;
	FontFamilyProperty->GetValue(FontObjectPtr);
	
	if (const UFont* FontObject = Cast<UFont>(FontObjectPtr))
	{
		const FName ActiveFontEntry = GetActiveFontEntry();
		TSharedPtr<FName> SelectedNamePtr;

		for (const FTypefaceEntry& TypefaceEntry : FontObject->CompositeFont.DefaultTypeface.Fonts)
		{
			TSharedPtr<FName> NameEntryPtr = MakeShareable(new FName(TypefaceEntry.Name));
			FontEntryComboData.Add(NameEntryPtr);

			if (!TypefaceEntry.Name.IsNone() && TypefaceEntry.Name == ActiveFontEntry)
			{
				SelectedNamePtr = NameEntryPtr;
			}
		}

		FontEntryComboData.Sort([](const TSharedPtr<FName>& One, const TSharedPtr<FName>& Two) -> bool
		{
			return One->ToString() < Two->ToString();
		});

		FontEntryCombo->ClearSelection();
		FontEntryCombo->RefreshOptions();
		FontEntryCombo->SetSelectedItem(SelectedNamePtr);
	}
	else
	{
		FontEntryCombo->ClearSelection();
		FontEntryCombo->RefreshOptions();
	}
}

void FDocumentationFontMappingsCustomization::OnFontEntrySelectionChanged(TSharedPtr<FName> InNewSelection, ESelectInfo::Type)
{
	if (InNewSelection.IsValid())
	{
		FName CurrentTypeface;
		TypefaceProperty->GetValue(CurrentTypeface);
		
		if (CurrentTypeface != *InNewSelection)
		{
			TypefaceProperty->SetValue(*InNewSelection);
		}
	}
}

TSharedRef<SWidget> FDocumentationFontMappingsCustomization::MakeFontEntryWidget(TSharedPtr<FName> InFontEntry)
{
	return SNew(STextBlock)
		.Text(FText::FromName(*InFontEntry))
		.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

FText FDocumentationFontMappingsCustomization::GetFontEntryComboText() const
{
	return FText::FromName(GetActiveFontEntry());
}

FName FDocumentationFontMappingsCustomization::GetActiveFontEntry() const
{
	UObject* FontObjectPtr = nullptr;
	FontFamilyProperty->GetValue(FontObjectPtr);
	
	if (const UFont* FontObject = Cast<UFont>(FontObjectPtr))
	{
		FName CurrentTypeface;
		TypefaceProperty->GetValue(CurrentTypeface);
		
		return (CurrentTypeface.IsNone() && FontObject->CompositeFont.DefaultTypeface.Fonts.Num())
			? FontObject->CompositeFont.DefaultTypeface.Fonts[0].Name
			: CurrentTypeface;
	}

	return NAME_None;
}

#undef LOCTEXT_NAMESPACE


