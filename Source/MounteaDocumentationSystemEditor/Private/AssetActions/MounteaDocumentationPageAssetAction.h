// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**
 * 
 */
class FMounteaDocumentationPageAssetAction : public FAssetTypeActions_Base
{
public:
	FMounteaDocumentationPageAssetAction();

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	//virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;
};
