// All rights reserved Dominik Morse 2024

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class SScrollBox;
class SMultiLineEditableText;
class UMounteaDocumentationPage;

class FMounteaDocumentationPageEditor : public FAssetEditorToolkit
{
public:
	void InitMounteaDocumentationEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* Object);

	// IAssetEditorInstance overrides
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	FText GenerateLineNumbers() const;
	FReply HandleTabPress(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent);

	void SpawnEditorTextWidget();

private:
	TSharedRef<SDockTab> SpawnMarkdownTab(const FSpawnTabArgs& Args);
	

	TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& Args);
	
	TObjectPtr<UMounteaDocumentationPage> EditedPage;
	TSharedPtr<class IDetailsView> PropertyWidget;
	TSharedPtr<SMultiLineEditableText> EditableTextWidget;
};


