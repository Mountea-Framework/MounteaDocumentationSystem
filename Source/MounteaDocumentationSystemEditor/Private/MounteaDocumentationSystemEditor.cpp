#include "MounteaDocumentationSystemEditor.h"

#include "AssetToolsModule.h"
#include "Core/MounteaDocumentationPage.h"
#include "AssetActions/MounteaDocumentationPageAssetAction.h"
#include "DetailsEditor/MounteaDocumentationPageCustomization.h"

DEFINE_LOG_CATEGORY(MounteaDocumentationSystemEditor);

#define LOCTEXT_NAMESPACE "FMounteaDocumentationSystemEditor"

void FMounteaDocumentationSystemEditor::StartupModule()
{
	// Register Custom Detail Panels
	/*
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		TArray<FOnGetDetailCustomizationInstance> CustomClassLayouts =
		{
			FOnGetDetailCustomizationInstance::CreateStatic(&FMounteaDocumentationPageCustomization::MakeInstance),
		};
		RegisteredCustomClassLayouts =
		{
			UMounteaDocumentationPage::StaticClass()->GetFName(),
		};
		for (int32 i = 0; i < RegisteredCustomClassLayouts.Num(); i++)
		{
			PropertyModule.RegisterCustomClassLayout(RegisteredCustomClassLayouts[i], CustomClassLayouts[i]);
		}
	}
	*/
	
	// Asset Actions
	{
		AssetActions.Add(MakeShared<FMounteaDocumentationPageAssetAction>());

		for (const auto& Itr : AssetActions)
		{
			FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(Itr.ToSharedRef());
		}
	}
	
	UE_LOG(MounteaDocumentationSystemEditor, Warning, TEXT("MounteaDocumentationSystemEditor module has been loaded"));
}

void FMounteaDocumentationSystemEditor::ShutdownModule()
{
	UE_LOG(MounteaDocumentationSystemEditor, Warning, TEXT("MounteaDocumentationSystemEditor module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaDocumentationSystemEditor, MounteaDocumentationSystemEditor)