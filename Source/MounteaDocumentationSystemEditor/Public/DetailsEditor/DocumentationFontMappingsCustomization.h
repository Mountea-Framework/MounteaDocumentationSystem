// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"
#include "Widgets/Input/SComboBox.h"

class FDocumentationFontMappingsCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	static bool OnFilterFontAsset(const FAssetData& InAssetData);
	void OnFontChanged(const FAssetData& InAssetData);
	
	bool IsFontEntryComboEnabled() const;
	void OnFontEntryComboOpening();
	void OnFontEntrySelectionChanged(TSharedPtr<FName> InNewSelection, ESelectInfo::Type);
	TSharedRef<SWidget> MakeFontEntryWidget(TSharedPtr<FName> InFontEntry);
	FText GetFontEntryComboText() const;
	FName GetActiveFontEntry() const;

	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	TSharedPtr<IPropertyHandle> FontFamilyProperty;
	TSharedPtr<IPropertyHandle> TypefaceProperty;
	TSharedPtr<IPropertyHandle> SizeProperty;
	TSharedPtr<IPropertyHandle> OverrideColorProperty;
	TSharedPtr<IPropertyHandle> ColorProperty;
	
	TSharedPtr<SComboBox<TSharedPtr<FName>>> FontEntryCombo;
	TArray<TSharedPtr<FName>> FontEntryComboData;
};
