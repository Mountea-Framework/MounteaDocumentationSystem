#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(MounteaDocumentationSystemEditor, All, All);

class FMounteaDocumentationSystemEditor : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	TArray<FName> RegisteredCustomClassLayouts;
	TArray<TSharedPtr<class FAssetTypeActions_Base>> AssetActions;
};