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

bool UMounteaDocumentationSystemStatics::HasUnmatchedMarker(const FString& Text, const TCHAR* Marker)
{
	int32 count = 0;
	int32 searchPos = 0;
	while (true)
	{
		int32 found = Text.Find(Marker, ESearchCase::CaseSensitive, ESearchDir::FromStart, searchPos);
		if (found == INDEX_NONE) break;
		count++;
		searchPos = found + FCString::Strlen(Marker);
	}
	return (count % 2) != 0; 
}

bool UMounteaDocumentationSystemStatics::ShouldSkipRegex(const FString& Line, int32 MatchStart)
{
	if (HasUnmatchedMarker(Line.Left(MatchStart), TEXT("**"))) return true;
	if (HasUnmatchedMarker(Line.Left(MatchStart), TEXT("`")))  return true;
	return false;
}

FString UMounteaDocumentationSystemStatics::ReplaceAllMatches(FString InLine, const FRegexPattern& Pattern, const FString& TagName)
{
	FRegexMatcher regexMatcher(Pattern, InLine);
	while (regexMatcher.FindNext())
	{
		const int32 MS = regexMatcher.GetMatchBeginning();
		const int32 ME = regexMatcher.GetMatchEnding();
		if (ShouldSkipRegex(InLine, MS))
			continue;

		const FString Pre = InLine.Left(MS);
		const FString Post = InLine.Mid(ME);
		const FString Captured = regexMatcher.GetCaptureGroup(1);
		const FString Replacement = FString::Printf(TEXT("<RichTextBlock.Mountea.%s>%s</>"), *TagName, *Captured);

		InLine = Pre + Replacement + Post;
		regexMatcher = FRegexMatcher(Pattern, InLine);
	}
	return InLine;
}

FString UMounteaDocumentationSystemStatics::ConvertLine(FString Line)
{
	// Headers first. If one matches (and isn't skipped), return immediately.
	{
		static const struct
		{
			const TCHAR* Pattern;
			const TCHAR* TagName;
		} Headers[] =
		{
			{ MounteaMarkdownPatterns::header4Pattern, TEXT("Header4") },
			{ MounteaMarkdownPatterns::header3Pattern, TEXT("Header3") },
			{ MounteaMarkdownPatterns::header2Pattern, TEXT("Header2") },
			{ MounteaMarkdownPatterns::header1Pattern, TEXT("Header1") },
		};

		for (auto& H : Headers)
		{
			FRegexPattern P(H.Pattern);
			FRegexMatcher M(P, Line);
			if (M.FindNext())
			{
				const int32 MS = M.GetMatchBeginning();
				if (!ShouldSkipRegex(Line, MS))
				{
					const int32 ME = M.GetMatchEnding();
					const FString Pre = Line.Left(MS);
					const FString Post = Line.Mid(ME);
					const FString Captured = M.GetCaptureGroup(1);
					const FString Replacement = FString::Printf(TEXT("<RichTextBlock.Mountea.%s>%s</>"), H.TagName, *Captured);
					Line = Pre + Replacement + Post;
				}
				return Line;
			}
		}
	}

	// Inline patterns for non-header lines
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::boldPattern),   TEXT("Bold"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::italicPattern), TEXT("Italic"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::codeBlockPattern), TEXT("CodeBlock"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::codePattern),   TEXT("Code"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::linkPattern),   TEXT("Link"));

	return Line;
}

FString UMounteaDocumentationSystemStatics::ConvertMarkdownToRichText(const FString& Markdown)
{
	TArray<FString> Lines;
	Markdown.ParseIntoArray(Lines, TEXT("\n"));

	FString Output;
	for (const FString& line : Lines)
	{
		Output += ConvertLine(line) + TEXT("\n");
	}
	return Output.TrimEnd();
}
