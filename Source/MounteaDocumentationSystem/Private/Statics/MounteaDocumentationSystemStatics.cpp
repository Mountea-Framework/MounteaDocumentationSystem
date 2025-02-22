// All rights reserved Dominik Morse 2024


#include "Statics/MounteaDocumentationSystemStatics.h"

#include "Settings/MounteaDocumentationSystemSettings.h"

/*
FString UMounteaDocumentationSystemStatics::ConvertMarkdownToRichText(const FString& Markdown)
{
   TArray<FString> Lines;
   Markdown.ParseIntoArray(Lines, TEXT("\n"));
   
   FString RichText;
   for (const FString& Line : Lines)
   {
	   FString ProcessedLine = Line;
	   
	   // Process bold first (2 stars)
	   while (ProcessedLine.Contains(TEXT("**")))
	   {
		   int32 StartIdx = ProcessedLine.Find(TEXT("**"));
		   int32 EndIdx = ProcessedLine.Find(TEXT("**"), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIdx + 2);
		   
		   if (EndIdx != INDEX_NONE)
		   {
			   ProcessedLine = ProcessedLine.Left(StartIdx) + 
				   TEXT("<RichTextBlock.Mountea.Bold>") + 
				   ProcessedLine.Mid(StartIdx + 2, EndIdx - (StartIdx + 2)) + 
				   TEXT("</>") + 
				   ProcessedLine.RightChop(EndIdx + 2);
		   }
		   else break;
	   }
	   
	   // Process italic (single star)
	   while (ProcessedLine.Contains(TEXT("*")))
	   {
		   int32 StartIdx = ProcessedLine.Find(TEXT("*"));
		   int32 EndIdx = ProcessedLine.Find(TEXT("*"), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIdx + 1);
		   
		   if (EndIdx != INDEX_NONE)
		   {
			   ProcessedLine = ProcessedLine.Left(StartIdx) + 
				   TEXT("<RichTextBlock.Mountea.Italic>") + 
				   ProcessedLine.Mid(StartIdx + 1, EndIdx - (StartIdx + 1)) + 
				   TEXT("</>") + 
				   ProcessedLine.RightChop(EndIdx + 1);
		   }
		   else break;
	   }

	// Process codeBlock (backticks)
	while (ProcessedLine.Contains(TEXT("```")))
	{
		int32 StartIdx = ProcessedLine.Find(TEXT("```"));
		int32 EndIdx = ProcessedLine.Find(TEXT("```"), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIdx + 3);
			   
		if (EndIdx != INDEX_NONE)
		{
			ProcessedLine = ProcessedLine.Left(StartIdx) + 
					   TEXT("<RichTextBlock.Mountea.CodeBlock>") + 
					   ProcessedLine.Mid(StartIdx + 3, EndIdx - (StartIdx + 3)) + 
					   TEXT("</>") + 
					   ProcessedLine.RightChop(EndIdx + 3);
		}
		else break;
	}
   	
		// Process code (backticks)
	   while (ProcessedLine.Contains(TEXT("`")))
	   {
		   int32 StartIdx = ProcessedLine.Find(TEXT("`"));
		   int32 EndIdx = ProcessedLine.Find(TEXT("`"), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIdx + 1);
		   
		   if (EndIdx != INDEX_NONE)
		   {
			   ProcessedLine = ProcessedLine.Left(StartIdx) + 
				   TEXT("<RichTextBlock.Mountea.Code>") + 
				   ProcessedLine.Mid(StartIdx + 1, EndIdx - (StartIdx + 1)) + 
				   TEXT("</>") + 
				   ProcessedLine.RightChop(EndIdx + 1);
		   }
		   else break;
	   }
	   
	   RichText += ProcessedLine + TEXT("\n");
   }
   
   return RichText.TrimEnd();
}
*/

FString UMounteaDocumentationSystemStatics::ConvertMarkdownToRichText(const FString& Markdown)
{
	UMounteaDocumentationSystemSettings* Settings = GetMutableDefault<UMounteaDocumentationSystemSettings>();
	
	FString ProcessedText = Markdown;
	
	for (const auto& tokenType : Settings->TextTypes)
	{
		FString OpenTag = FString::Printf(TEXT("<RichTextBlock.Mountea.%s>"), *tokenType.ToString());
		FString CloseTag = TEXT("</>");
		FString tokenSanitized = tokenType.ToString();
		tokenSanitized.RemoveSpacesInline();

		bool bIsHeader = false;
		
		FRegexPattern regexPattern(TEXT(""));

		if (tokenType == TEXT("Header 4"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::header4Pattern);
			bIsHeader = true;
		}
		if (tokenType == TEXT("Header 3"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::header3Pattern);
			bIsHeader = true;
		}
		if (tokenType == TEXT("Header 2"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::header2Pattern);
			bIsHeader = true;
		}
		if (tokenType == TEXT("Header 1"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::header1Pattern);
			bIsHeader = true;
		}

		if (bIsHeader)
		{
			ProcessedText = ConvertMarkdownToRichTextPerType(ProcessedText, regexPattern, tokenSanitized);
			continue;
		} 

		if (tokenType == TEXT("Bold"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::boldPattern);
		}
		else if (tokenType == TEXT("Italic"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::italicPattern);
		}
		else if (tokenType == TEXT("CodeBlock"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::codeBlockPattern);
		}
		else if (tokenType == TEXT("Code"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::codePattern);
		}
		else if (tokenType == TEXT("Link"))
		{
			regexPattern = FRegexPattern(MounteaMarkdownPatterns::linkPattern);
		}
		else
		{
			continue;
		}

		ProcessedText = ConvertMarkdownToRichTextPerType(ProcessedText, regexPattern, tokenSanitized);
	}
	
	return ProcessedText.TrimEnd();
}

FString UMounteaDocumentationSystemStatics::ConvertMarkdownToRichTextPerType(const FString& Markdown,
	const FRegexPattern& Pattern, const FString& Token)
{
	// Use a working copy of the Markdown text.
	FString ProcessedText = Markdown;

	FString OpenTag = FString::Printf(TEXT("<RichTextBlock.Mountea.%s>"), *Token);
	FString CloseTag = TEXT("</>");

	FString NewText;
	int32 LastIndex = 0;
	FRegexMatcher regexMatcher(Pattern, ProcessedText);

	while (regexMatcher.FindNext())
	{
		int32 MatchStart = regexMatcher.GetMatchBeginning();
		int32 MatchEnd = regexMatcher.GetMatchEnding();

		if (MatchStart > LastIndex)
		{
			NewText += ProcessedText.Mid(LastIndex, MatchStart - LastIndex);
		}
		
		FString TokenContent = regexMatcher.GetCaptureGroup(1);
		NewText += OpenTag + TokenContent + CloseTag;
		LastIndex = MatchEnd;
	}
	
	if (LastIndex < ProcessedText.Len())
	{
		NewText += ProcessedText.Mid(LastIndex);
	}
		
	// Update the processed text for the next token type.
	ProcessedText = NewText;

	return ProcessedText;
}
