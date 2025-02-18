// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MounteaDocumentationContainer.generated.h"

class UMounteaDocumentationPage;

/**
 * 
 */
UCLASS()
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaDocumentationContainer : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Documentation")
	TMap<int32, TObjectPtr<UMounteaDocumentationPage>> Pages;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Documentation")
	int32 ActivePageIndex = 0;
};
