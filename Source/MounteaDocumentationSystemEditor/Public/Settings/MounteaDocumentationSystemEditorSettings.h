// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MounteaDocumentationSystemEditorSettings.generated.h"

/**
 * 
 */
UCLASS(config = MounteaSettings, DefaultConfig, ProjectUserConfig)
class MOUNTEADOCUMENTATIONSYSTEMEDITOR_API UMounteaDocumentationSystemEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UMounteaDocumentationSystemEditorSettings();

	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("MounteaDocumentationEditorSystem", "MounteaSettingsEditorSection", "Mountea Documentation System (Editor)");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("MounteaDocumentationEditorSystem", "MounteaSettingsEditorDescription", "Default values for Mountea Plugins (Editor).");
	}

	virtual FName GetContainerName() const override
	{
		return "Project";
	}

public:

	FSlateFontInfo GetEditorFont() const;
	
public:

	UPROPERTY(config, EditDefaultsOnly, Category="Font")
	TSoftObjectPtr<UFont> FontFamily;

	UPROPERTY(config, EditDefaultsOnly, Category="Font", meta=(GetOptions ="GetFontfaces"))
	FName Typeface;
	
	UPROPERTY(config, EditDefaultsOnly, Category="Font", meta=(ClampMin=1, ClampMax=1000))
	int32 Size;
	
	UPROPERTY(config, BlueprintReadOnly, Category="Font")
	FSlateFontInfo EditorFont;

	UPROPERTY(config, EditDefaultsOnly, Category="Font", meta=(UIMin=1.f,ClampMin=1.f))
	int EstimatedWordPerLine = 85;

private:

	UFUNCTION()
	TArray<FName> GetFontfaces() const;

	FSlateFontInfo CreateSlateFont() const;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
