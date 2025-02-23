// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Internationalization/Regex.h"
#include "MounteaDocumentationSystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEADOCUMENTATIONSYSTEM_API UMounteaDocumentationSystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool ShouldSkipRegex(const FString& Line, int32 MatchStart);
	static FString ConvertMarkdownToRichText(const FString& Markdown);
	static bool HasUnmatchedMarker(const FString& Text, const TCHAR* Marker);
	static FString ReplaceAllMatches(FString InLine, const FRegexPattern& Pattern, const FString& TagName);
	static FString ConvertLine(FString Line);

};

namespace MounteaMarkdownPatterns
{
	inline const TCHAR* boldPattern = TEXT("\\*\\*([\\s\\S]+?)\\*\\*");
	inline const TCHAR* italicPattern = TEXT("(?<!\\*)\\*([\\s\\S]+?)\\*(?!\\*)");
	static const TCHAR* codeBlockPattern = TEXT("```([\\s\\S]+?)```");
	inline const TCHAR* codePattern = TEXT("`([^`]+?)`");
	inline const TCHAR* header1Pattern = TEXT("(?m)^#\\s+([\\s\\S]+?)$");
	inline const TCHAR* header2Pattern = TEXT("(?m)^##\\s+([\\s\\S]+?)$");
	inline const TCHAR* header3Pattern = TEXT("(?m)^###\\s+([\\s\\S]+?)$");
	inline const TCHAR* header4Pattern = TEXT("(?m)^####\\s+([\\s\\S]+?)$");
	inline const TCHAR* linkPattern = TEXT("\\[([^\\]]+?)\\]\\(([^\\)\\s]+)(?:\\s+\"([^\"]+?)\")?\\)");
}

