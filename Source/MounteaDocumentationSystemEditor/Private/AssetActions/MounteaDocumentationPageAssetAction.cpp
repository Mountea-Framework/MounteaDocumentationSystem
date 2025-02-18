// All rights reserved Dominik Morse 2024


#include "MounteaDocumentationPageAssetAction.h"

#include "Core/MounteaDocumentationPage.h"
#include "DetailsEditor/MounteaDocumentationPageEditor.h"

#define LOCTEXT_NAMESPACE "MounteaDocumentationPageAssetAction"

FMounteaDocumentationPageAssetAction::FMounteaDocumentationPageAssetAction()
{
}

FText FMounteaDocumentationPageAssetAction::GetName() const
{
	return LOCTEXT("MounteaDialogueGraphAssetAction", "Mountea Dialogue Tree");
}

/*
FText FMounteaDocumentationPageAssetAction::GetAssetDescription(const FAssetData& AssetData) const
{
	return LOCTEXT("MounteaDialogueGraphAssetAction", "Mountea Dialogue Tree Asset for creating Mountea Dialogues.");
}
*/

FColor FMounteaDocumentationPageAssetAction::GetTypeColor() const
{
	return FColor::Orange;
}

UClass* FMounteaDocumentationPageAssetAction::GetSupportedClass() const
{
	return UMounteaDocumentationPage::StaticClass();
}

void FMounteaDocumentationPageAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UMounteaDocumentationPage* Page = Cast<UMounteaDocumentationPage>(*ObjIt))
		{
			TSharedRef<FMounteaDocumentationPageEditor> NewPageEditor = MakeShared<FMounteaDocumentationPageEditor>();
			NewPageEditor->InitMounteaDocumentationEditor(Mode, EditWithinLevelEditor, Page);
		}
	}
}

uint32 FMounteaDocumentationPageAssetAction::GetCategories()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Mountea Documentation"));
	}

	return EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE