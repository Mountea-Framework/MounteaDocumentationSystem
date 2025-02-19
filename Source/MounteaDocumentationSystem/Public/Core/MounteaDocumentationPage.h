// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaDocumentationPage.generated.h"

UENUM(BlueprintType)
enum class EMounteaDocumentationPageType : uint8
{
	EMDPT_External UMETA(DisplayName="External File"),
	EMDPT_Editor		UMETA(DisplayName="Editor Text"),

	Default					UMETA(hidden)
};

/**
 * 
 */
UCLASS()
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaDocumentationPage : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	EMounteaDocumentationPageType PageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source", meta=(FilePathFilter = "Markdown File (*.md)|*.md"), meta=(EditConditionHides, EditCondition="PageType==EMounteaDocumentationPageType::EMDPT_External"))
	FFilePath MarkdownFile;

	UPROPERTY(BlueprintReadOnly, Category="Context", meta=(MultiLine=true))
	FText PageContent;

	UPROPERTY(BlueprintReadOnly, Category="Context", meta=(MultiLine=true))
	FText RichTextPageContent;
};

USTRUCT(BlueprintType)
struct FMounteaDocumentationPageWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Page")
	FText PageTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Page")
	TObjectPtr<UMounteaDocumentationPage> Page;
};
