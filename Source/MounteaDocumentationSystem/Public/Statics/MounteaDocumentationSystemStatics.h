// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaDocumentationSystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaDocumentationSystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FString ConvertMarkdownToRichText(const FString& Markdown);

};
