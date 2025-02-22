// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MounteaDocumentationSystemSettings.generated.h"

class UFont;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDocumentationPreviewFontChanged);

USTRUCT(BlueprintType)
struct FDocumentationFontMappings
{
	GENERATED_BODY()

	UPROPERTY(config, EditDefaultsOnly, Category="PreviewFont")
	TSoftObjectPtr<UFont> FontFamily;

	UPROPERTY(config, EditDefaultsOnly, Category="PreviewFont", meta=(GetOptions ="GetFontfaces"))
	FName Typeface;
	
	UPROPERTY(config, EditDefaultsOnly, Category="PreviewFont", meta=(ClampMin=1, ClampMax=1000))
	int32 Size;
	
	UPROPERTY(config, BlueprintReadOnly, Category="PreviewFont")
	FSlateFontInfo PreviewFont;
};

/**
 * 
 */
UCLASS(config = MounteaSettings, DefaultConfig, ProjectUserConfig)
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaDocumentationSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UMounteaDocumentationSystemSettings();

#if WITH_EDITOR
public:

	virtual FText GetSectionText() const override
	{
		return NSLOCTEXT("MounteaDocumentationSystem", "MounteaSettingsSection", "Mountea Documentation System");
	}

	virtual FText GetSectionDescription() const override
	{
		return NSLOCTEXT("MounteaDocumentationSystem", "MounteaSettingsDescription", "Default values for Mountea Plugins.");
	}

	virtual FName GetContainerName() const override
	{
		return "Project";
	}

	UFUNCTION(CallInEditor, Category="Defaults")
	void SetDefaultTextTypes();

#endif

protected:

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category="Documentation")
	FOnDocumentationPreviewFontChanged OnDocumentationPreviewFontChanged;

public:

	UFUNCTION()
	void RefreshPreviewFonts();

	FSlateFontInfo GetFont(const FName& Type) const;
	
public:

	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category="PreviewFont")
	FLinearColor FontColor = FLinearColor::White;

	/**
	 * header1, header2 etc.
	 */
	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category="PreviewFont")
	TSet<FName>TextTypes;
	
	UPROPERTY(config, BlueprintReadOnly, EditAnywhere, Category="PreviewFont", meta=(ShowOnlyInnerProperties, ForceInlineRow), meta=(GetKeyOptions="GetTextTypes"))
	TMap<FName,FDocumentationFontMappings> FontMappings;

private:
	
	UFUNCTION()
	TArray<FName> GetTextTypes() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;	
#endif
	
};
