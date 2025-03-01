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
	
	/**
	 * Converts the provided Markdown text into a rich text format suitable for use in a URichTextBlock.
	 *
	 * @param Markdown The raw Markdown string to be converted.
	 * @return A string containing rich text markup based on the Markdown input.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Utilities|Markdown", meta=(CustomTag="MounteaK2Getter"))
	static FString ConvertMarkdownToRichText(const FString& Markdown);
	static bool ShouldSkipRegex(const FString& Line, int32 MatchStart);	
	static bool HasUnmatchedMarker(const FString& Text, const TCHAR* Marker);
	static FString ReplaceAllMatches(FString InLine, const FRegexPattern& Pattern, const FString& TagName);
	static FString ConvertLine(FString Line);

public:

	/**
	 * Wraps raw HTML content in a complete HTML page structure
	 *
	 * @param RawHTML The HTML content to be wrapped
	 * @return A string containing a complete HTML document
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Utilities|HTML", meta=(CustomTag="MounteaK2Getter"))
	static FString RawHTMLToPage(const FString& RawHTML);
	
	/**
	 * Converts the provided Markdown text into HTML format.
	 *
	 * @param Markdown The raw Markdown string to be converted.
	 * @return A string containing HTML markup based on the Markdown input.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Utilities|Markdown", meta=(CustomTag="MounteaK2Getter"))
	static FString ConvertMarkdownToHTML(const FString& Markdown);
	static void IdentifyHTMLBlocks(const TArray<FString>& Lines, TArray<bool>& IsHTMLLine);
	static void ProcessCodeBlocks(TArray<FString>& Lines, TArray<bool>& IsHTMLLine, TMap<int32, FString>& CodeBlocks);
	static FString FormatCodeBlock(const FString& Content, const FString& Language);
	static FString BuildHTML(const TArray<FString>& Lines, const TArray<bool>& IsHTMLLine, const TMap<int32, FString>& CodeBlocks);
	static FString ProcessInlineElements(FString Content);
	static FString ProcessBadgeLine(const FString& Line);
	static FString ProcessBadgeLinks(FString Content);
	static FString ProcessImages(FString Content);
	static FString ProcessLinks(FString Content);
	static FString ProcessTextFormatting(FString Content);
	static FString ReplacePattern(const FString& Content, const FString& Pattern, const FString& Replacement);
	static FString ProcessHeaderLine(const FString& Line);
	static FString ProcessListBlock(const TArray<FString>& Lines, const int32 Start, const int32 End);
	static FString ProcessInlineTextForItem(const FString& Text);
	static FString ProcessBlockquoteLine(const FString& Line);
	static bool IsOrderedListItem(const FString& Line);
	static bool IsTableRow(const FString& Line);
	static bool IsTableSeparator(const FString& Line);
	static TArray<FString> ParseTableRow(const FString& Line);
	static FString ProcessTable(const TArray<FString>& Lines, int32& Start, TArray<bool>& IsHTMLLine);
	
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

namespace MounteaMarkdownHTMLPatterns
{
	// Inline formatting
	inline const TCHAR* BoldPattern = TEXT("\\*\\*([\\s\\S]+?)\\*\\*");
	inline const TCHAR* ItalicPattern = TEXT("(?<!\\*)\\*([\\s\\S]+?)\\*(?!\\*)");
	inline const TCHAR* CodeBlockPattern = TEXT("```([\\s\\S]+?)```");
	inline const TCHAR* InlineCodePattern = TEXT("`([^`]+?)`");
    
	// Headers
	inline const TCHAR* Header1Pattern = TEXT("(?m)^#\\s+([\\s\\S]+?)$");
	inline const TCHAR* Header2Pattern = TEXT("(?m)^##\\s+([\\s\\S]+?)$");
	inline const TCHAR* Header3Pattern = TEXT("(?m)^###\\s+([\\s\\S]+?)$");
	inline const TCHAR* Header4Pattern = TEXT("(?m)^####\\s+([\\s\\S]+?)$");
	inline const TCHAR* Header5Pattern = TEXT("(?m)^#####\\s+([\\s\\S]+?)$");
	inline const TCHAR* Header6Pattern = TEXT("(?m)^######\\s+([\\s\\S]+?)$");
    
	// Links and images
	inline const TCHAR* LinkPattern = TEXT("\\[([^\\]]+?)\\]\\(([^\\)\\s]+)(?:\\s+\"([^\"]+?)\")?\\)");
	inline const TCHAR* ImagePattern = TEXT("!\\[([^\\]]*?)\\]\\(([^\\)\\s]+)(?:\\s+\"([^\"]+?)\")?\\)");
    
	// Lists
	inline const TCHAR* UnorderedListPattern = TEXT("^-\\s+(.+)$");
	inline const TCHAR* OrderedListPattern = TEXT("^\\d+\\.\\s+(.+)$");
    
	// Blockquotes
	inline const TCHAR* BlockquotePattern = TEXT("^>\\s+(.+)$");
    
	// Horizontal rule
	inline const TCHAR* HorizontalRulePattern = TEXT("^(?:\\*\\*\\*|---|___)\\s*$");
}