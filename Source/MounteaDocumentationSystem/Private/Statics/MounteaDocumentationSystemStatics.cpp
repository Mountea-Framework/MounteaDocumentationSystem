// All rights reserved Dominik Morse 2024


#include "Statics/MounteaDocumentationSystemStatics.h"

#include "Settings/MounteaDocumentationSystemSettings.h"

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

bool UMounteaDocumentationSystemStatics::ShouldSkipRegex(const FString& Line, const int32 MatchStart)
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
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::boldPattern),	TEXT("Bold"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::italicPattern), TEXT("Italic"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::codeBlockPattern), TEXT("CodeBlock"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::codePattern),	TEXT("Code"));
	Line = ReplaceAllMatches(Line, FRegexPattern(MounteaMarkdownPatterns::linkPattern),	TEXT("Link"));

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



FString UMounteaDocumentationSystemStatics::RawHTMLToPage(const FString& RawHTML)
{
	const auto newStyle = GetDefault<UMounteaDocumentationSystemSettings>();
	return FString::Printf(TEXT(R"(
	 <html>
		  <head>
				<meta charset='UTF-8'>
				<title>Mountea Documentation</title>
				<style>
				%s
				</style>
		  </head>
		  <body>
				<main>
				%s
				</main>
		  </body>
	 </html>)"), *newStyle->DisplayCSS, *RawHTML);
}

FString UMounteaDocumentationSystemStatics::ConvertMarkdownToHTML(const FString& Markdown) 
{
	// Stage 1: Split content into lines
	TArray<FString> Lines;
	Markdown.ParseIntoArrayLines(Lines, false);
	
	// Stage 2: Identify and mark HTML blocks that should be preserved
	TArray<bool> IsHTMLLine;
	IsHTMLLine.Init(false, Lines.Num());
	IdentifyHTMLBlocks(Lines, IsHTMLLine);
	
	// Stage 3: Process badges first (high priority)
	for (int32 i = 0; i < Lines.Num(); i++)
	{
		if (!IsHTMLLine[i] && Lines[i].TrimStartAndEnd().StartsWith(TEXT("[![")))
		{
			Lines[i] = ProcessBadgeLine(Lines[i]);
			IsHTMLLine[i] = true;
		}
	}
	
	// Stage 4: Process code blocks
	TMap<int32, FString> CodeBlocks;
	ProcessCodeBlocks(Lines, IsHTMLLine, CodeBlocks);
	
	// Stage 5: Build HTML with preserved blocks
	FString ProcessedHTML = BuildHTML(Lines, IsHTMLLine, CodeBlocks);
	
	// Stage 6: Process remaining inline elements
	ProcessedHTML = ProcessInlineElements(ProcessedHTML);
	
	return ProcessedHTML.TrimEnd();
}

void UMounteaDocumentationSystemStatics::IdentifyHTMLBlocks(const TArray<FString>& Lines, TArray<bool>& IsHTMLLine)
{
	static const TArray<FString> HTMLTags = {
		TEXT("<p"), TEXT("<div"), TEXT("<span"), TEXT("<img"), TEXT("<a"), 
		TEXT("<ul"), TEXT("<ol"), TEXT("<li"), TEXT("<table"), TEXT("<tr"), 
		TEXT("<td"), TEXT("<th"), TEXT("<pre"), TEXT("<code"), TEXT("<h1"), 
		TEXT("<h2"), TEXT("<h3"), TEXT("<h4"), TEXT("<h5"), TEXT("<h6"),
		TEXT("<blockquote"), TEXT("<iframe"), TEXT("<figure")
	};
	
	for (int32 i = 0; i < Lines.Num(); i++)
	{
		const FString TrimmedLine = Lines[i].TrimStartAndEnd();
		
		if (IsHTMLLine[i])
			continue;
		
		bool IsHTML = false;
		for (const FString& Tag : HTMLTags)
		{
			if (TrimmedLine.StartsWith(Tag))
			{
				IsHTML = true;
				break;
			}
		}
		
		if (IsHTML)
		{
			int32 TagEnd = TrimmedLine.Find(TEXT(">"));
			if (TagEnd != INDEX_NONE)
			{
				FString TagName;
				int32 SpacePos = TrimmedLine.Find(TEXT(" "));
				if (SpacePos != INDEX_NONE && SpacePos < TagEnd)
				{
					TagName = TrimmedLine.Mid(1, SpacePos - 1);
				}
				else
				{
					TagName = TrimmedLine.Mid(1, TagEnd - 1);
				}
				
				IsHTMLLine[i] = true;
				
				if (TrimmedLine.EndsWith(TEXT("/>")) || 
					TagName.Equals(TEXT("img")) || 
					TagName.Equals(TEXT("br")) || 
					TagName.Equals(TEXT("hr")))
				{
					continue;
				}
				
				FString ClosingTag = FString::Printf(TEXT("</%s>"), *TagName);
				
				if (TrimmedLine.Contains(ClosingTag))
				{
					continue;
				}
				
				for (int32 j = i + 1; j < Lines.Num(); j++)
				{
					if (Lines[j].Contains(ClosingTag))
					{
						for (int32 k = i + 1; k <= j; k++)
						{
							IsHTMLLine[k] = true;
						}
						break;
					}
				}
			}
		}
	}
}

void UMounteaDocumentationSystemStatics::ProcessCodeBlocks(TArray<FString>& Lines, TArray<bool>& IsHTMLLine, TMap<int32, FString>& CodeBlocks)
{
	bool InCodeBlock = false;
	int32 CodeBlockStart = -1;
	FString CodeBlockLanguage;
	FString CodeBlockContent;
	
	for (int32 i = 0; i < Lines.Num(); i++)
	{
		if (IsHTMLLine[i])
			continue;
			
		const FString& Line = Lines[i].TrimStartAndEnd();
		
		if (Line.StartsWith(TEXT("```")))
		{
			if (!InCodeBlock)
			{
				CodeBlockStart = i;
				CodeBlockLanguage = Line.RightChop(3).TrimStartAndEnd();
				InCodeBlock = true;
				CodeBlockContent = TEXT("");
				IsHTMLLine[i] = true;
			}
			else
			{
				InCodeBlock = false;
				
				FString FormattedBlock = FormatCodeBlock(CodeBlockContent, CodeBlockLanguage);
				
				CodeBlocks.Add(CodeBlockStart, FormattedBlock);
				
				for (int32 j = CodeBlockStart; j <= i; j++)
				{
					IsHTMLLine[j] = true;
				}
			}
		}
		else if (InCodeBlock)
		{
			if (!CodeBlockContent.IsEmpty())
			{
				CodeBlockContent += TEXT("\n");
			}
			
			CodeBlockContent += Lines[i];
			IsHTMLLine[i] = true;
		}
	}
}

FString UMounteaDocumentationSystemStatics::FormatCodeBlock(const FString& Content, const FString& Language)
{
	// Escape HTML characters
	FString SafeContent = Content;
	SafeContent.ReplaceInline(TEXT("&"), TEXT("&amp;"));
	SafeContent.ReplaceInline(TEXT("<"), TEXT("&lt;"));
	SafeContent.ReplaceInline(TEXT(">"), TEXT("&gt;"));
	
	TArray<FString> ContentLines;
	SafeContent.ParseIntoArrayLines(ContentLines, false);
	
	FString LangClass = Language.IsEmpty() ? TEXT("") : FString::Printf(TEXT(" class=\"language-%s\""), *Language);
	FString Result = TEXT("<pre>\n");
	
	for (int32 i = 0; i < ContentLines.Num(); i++)
	{
		if (i > 0)
		{
			Result += TEXT("\n");
		}
		
		Result += FString::Printf(TEXT("\t<code>%s</code>"), *ContentLines[i]);
	}
	
	Result += TEXT("\n</pre>");
	return Result;
}

FString UMounteaDocumentationSystemStatics::BuildHTML(const TArray<FString>& Lines, const TArray<bool>& IsHTMLLine, const TMap<int32, FString>& CodeBlocks)
{
	FString Result;
	int32 i = 0;
	bool InParagraph = false;
	
	while (i < Lines.Num())
	{
		if (CodeBlocks.Contains(i))
		{
			if (InParagraph)
			{
				Result += TEXT("</p>\n");
				InParagraph = false;
			}
			
			Result += CodeBlocks[i] + TEXT("\n");
			
			int32 j = i + 1;
			while (j < Lines.Num() && IsHTMLLine[j])
			{
				j++;
			}
			
			i = j;
			continue;
		}
		
		if (IsHTMLLine[i])
		{
			if (InParagraph)
			{
				Result += TEXT("</p>\n");
				InParagraph = false;
			}
			
			Result += Lines[i] + TEXT("\n");
			i++;
			continue;
		}
		
		const FString& Line = Lines[i].TrimStartAndEnd();
		
		if (Line.IsEmpty())
		{
			// Close any open paragraph
			if (InParagraph)
			{
				Result += TEXT("</p>\n");
				InParagraph = false;
			}
			
			i++;
			continue;
		}
		
		// Process based on content type
		if (Line.StartsWith(TEXT("#")))
		{
			if (InParagraph)
			{
				Result += TEXT("</p>\n");
				InParagraph = false;
			}
			
			Result += ProcessHeaderLine(Line) + TEXT("\n");
		}
		else if (Line.StartsWith(TEXT("-")) || Line.StartsWith(TEXT("*")) || IsOrderedListItem(Line))
		{
			if (InParagraph)
			{
				Result += TEXT("</p>\n");
				InParagraph = false;
			}
			
			int32 ListStart = i;
			int32 ListEnd = i;
			
			while (ListEnd < Lines.Num() && !IsHTMLLine[ListEnd] && 
				  (Lines[ListEnd].TrimStartAndEnd().StartsWith(TEXT("-")) || 
					Lines[ListEnd].TrimStartAndEnd().StartsWith(TEXT("*")) ||
					IsOrderedListItem(Lines[ListEnd].TrimStartAndEnd())))
			{
				ListEnd++;
			}
			
			Result += ProcessListBlock(Lines, ListStart, ListEnd) + TEXT("\n");
			i = ListEnd;
			continue;
		}
		else if (Line.StartsWith(TEXT(">")))
		{
			if (InParagraph)
			{
				Result += TEXT("</p>\n");
				InParagraph = false;
			}
			
			Result += ProcessBlockquoteLine(Line) + TEXT("\n");
		}
		else
		{
			if (!InParagraph)
			{
				Result += TEXT("<p>");
				InParagraph = true;
			}
			
			Result += Line;
		}
		
		i++;
	}
	
	if (InParagraph)
	{
		Result += TEXT("</p>\n");
	}
	
	return Result;
}

FString UMounteaDocumentationSystemStatics::ProcessInlineElements(FString Content)
{
	Content = ProcessBadgeLinks(Content);

	Content = ProcessImages(Content);

	Content = ProcessLinks(Content);
 
	Content = ProcessTextFormatting(Content);
	
	return Content;
}

FString UMounteaDocumentationSystemStatics::ProcessBadgeLine(const FString& Line)
{
	// Handle badge-style markdown: [![text](img-url)](link-url)
	static const FRegexPattern BadgePattern(TEXT("\\[!\\[([^\\]]+)\\]\\(([^\\)]+)\\)\\]\\(([^\\)]+)\\)"));
	FRegexMatcher BadgeMatcher(BadgePattern, Line);
	
	if (BadgeMatcher.FindNext())
	{
		const FString AltText = BadgeMatcher.GetCaptureGroup(1);
		const FString ImgUrl = BadgeMatcher.GetCaptureGroup(2);
		const FString LinkUrl = BadgeMatcher.GetCaptureGroup(3);
		
		return FString::Printf(
			TEXT("<a href=\"%s\" target=\"blank\" rel=\"noopener noreferrer\"><img src=\"%s\" alt=\"%s\" style=\"max-width: 100%%;\"></a>"),
			*LinkUrl, *ImgUrl, *AltText
		);
	}
	
	return Line;
}

FString UMounteaDocumentationSystemStatics::ProcessBadgeLinks(FString Content)
{
	static const FRegexPattern BadgePattern(TEXT("\\[!\\[([^\\]]+)\\]\\(([^\\)]+)\\)\\]\\(([^\\)]+)\\)"));
	FRegexMatcher BadgeMatcher(BadgePattern, Content);
	
	TArray<TPair<FString, FString>> Replacements;
	
	while (BadgeMatcher.FindNext())
	{
		const int32 Start = BadgeMatcher.GetMatchBeginning();
		const int32 End = BadgeMatcher.GetMatchEnding();
		
		if (Start >= 0 && End > Start && End <= Content.Len())
		{
			const FString WholeMatch = Content.Mid(Start, End - Start);
			const FString AltText = BadgeMatcher.GetCaptureGroup(1);
			const FString ImgUrl = BadgeMatcher.GetCaptureGroup(2);
			const FString LinkUrl = BadgeMatcher.GetCaptureGroup(3);
			
			FString Replacement = FString::Printf(
				TEXT("<a href=\"%s\" target=\"blank\" rel=\"noopener noreferrer\"><img src=\"%s\" alt=\"%s\" style=\"max-width: 100%%;\"></a>"), 
				*LinkUrl, *ImgUrl, *AltText
			);
			
			Replacements.Add(TPair<FString, FString>(WholeMatch, Replacement));
		}
	}
	
	// Apply all replacements at once
	for (const auto& Pair : Replacements)
	{
		Content = Content.Replace(*Pair.Key, *Pair.Value, ESearchCase::CaseSensitive);
	}
	
	return Content;
}

FString UMounteaDocumentationSystemStatics::ProcessImages(FString Content)
{
	// Only process images that aren't already processed as part of badge links
	static const FRegexPattern ImagePattern(TEXT("!\\[([^\\]]+)\\]\\(([^\\)]+)\\)"));
	FRegexMatcher ImageMatcher(ImagePattern, Content);
	
	TArray<TPair<FString, FString>> Replacements;
	
	while (ImageMatcher.FindNext())
	{
		const int32 Start = ImageMatcher.GetMatchBeginning();
		const int32 End = ImageMatcher.GetMatchEnding();
		
		if (Start >= 0 && End > Start && End <= Content.Len())
		{
			const FString PrefixCheck = Start > 0 ? Content.Mid(Start - 1, 1) : TEXT("");
			if (PrefixCheck.Equals(TEXT("[")))
			{
				// This is likely part of a badge pattern, skip it
				continue;
			}
			
			const FString WholeMatch = Content.Mid(Start, End - Start);
			const FString AltText = ImageMatcher.GetCaptureGroup(1);
			const FString ImgUrl = ImageMatcher.GetCaptureGroup(2);
			
			FString Replacement = FString::Printf(
				TEXT("<img src=\"%s\" alt=\"%s\" style=\"max-width: 100%%;\">"), 
				*ImgUrl, *AltText
			);
			
			Replacements.Add(TPair<FString, FString>(WholeMatch, Replacement));
		}
	}
	
	for (const auto& Pair : Replacements)
	{
		Content = Content.Replace(*Pair.Key, *Pair.Value, ESearchCase::CaseSensitive);
	}
	
	return Content;
}

FString UMounteaDocumentationSystemStatics::ProcessLinks(FString Content)
{
	// Process regular links, but avoid those already processed as badges
	static const FRegexPattern LinkPattern(TEXT("\\[([^!\\[]*)\\]\\(([^\\)]+)\\)"));
	FRegexMatcher LinkMatcher(LinkPattern, Content);
	
	TArray<TPair<FString, FString>> Replacements;
	
	while (LinkMatcher.FindNext())
	{
		const int32 Start = LinkMatcher.GetMatchBeginning();
		const int32 End = LinkMatcher.GetMatchEnding();
		
		if (Start >= 0 && End > Start && End <= Content.Len())
		{
			const FString WholeMatch = Content.Mid(Start, End - Start);
			const FString LinkText = LinkMatcher.GetCaptureGroup(1);
			const FString LinkUrl = LinkMatcher.GetCaptureGroup(2);
			
			// Skip if this appears to be part of a badge pattern
			if (LinkText.Contains(TEXT("![")) || LinkText.StartsWith(TEXT("!")))
			{
				continue;
			}
			
			FString Replacement = FString::Printf(
				TEXT("<a href=\"%s\" target=\"blank\" rel=\"noopener noreferrer\">%s</a>"), 
				*LinkUrl, *LinkText
			);
			
			Replacements.Add(TPair<FString, FString>(WholeMatch, Replacement));
		}
	}
	
	for (const auto& Pair : Replacements)
	{
		Content = Content.Replace(*Pair.Key, *Pair.Value, ESearchCase::CaseSensitive);
	}
	
	return Content;
}

FString UMounteaDocumentationSystemStatics::ProcessTextFormatting(FString Content)
{
	Content = ReplacePattern(Content, TEXT("\\*\\*([^\\*]+)\\*\\*"), TEXT("<strong>$1</strong>"));
	Content = ReplacePattern(Content, TEXT("__([^_]+)__"), TEXT("<strong>$1</strong>"));
	
	Content = ReplacePattern(Content, TEXT("\\*([^\\*]+)\\*"), TEXT("<em>$1</em>"));
	Content = ReplacePattern(Content, TEXT("_([^_]+)_"), TEXT("<em>$1</em>"));
	
	Content = ReplacePattern(Content, TEXT("`([^`]+)`"), TEXT("<code>$1</code>"));
	
	return Content;
}

FString UMounteaDocumentationSystemStatics::ReplacePattern(const FString& Content, const FString& Pattern, const FString& Replacement)
{
	FRegexPattern RegexPattern(Pattern);
	FRegexMatcher Matcher(RegexPattern, Content);
	
	FString Result = Content;
	TArray<TPair<FString, FString>> Replacements;
	
	while (Matcher.FindNext())
	{
		const int32 Start = Matcher.GetMatchBeginning();
		const int32 End = Matcher.GetMatchEnding();
		
		if (Start >= 0 && End > Start && End <= Result.Len())
		{
			const FString WholeMatch = Result.Mid(Start, End - Start);
			FString NewText = Replacement;
			
			for (int32 i = 1; i <= 9; i++)
			{
				if (Matcher.GetCaptureGroupBeginning(i) != -1)
				{
					const FString Placeholder = FString::Printf(TEXT("$%d"), i);
					const FString CaptureText = Matcher.GetCaptureGroup(i);
					NewText = NewText.Replace(*Placeholder, *CaptureText, ESearchCase::CaseSensitive);
				}
			}
			
			Replacements.Add(TPair<FString, FString>(WholeMatch, NewText));
		}
	}
	
	for (const auto& Pair : Replacements)
	{
		Result = Result.Replace(*Pair.Key, *Pair.Value, ESearchCase::CaseSensitive);
	}
	
	return Result;
}

FString UMounteaDocumentationSystemStatics::ProcessHeaderLine(const FString& Line)
{
	int32 Level = 0;
	while (Level < Line.Len() && Line[Level] == '#')
	{
		Level++;
	}
	
	// Limit to h6
	Level = FMath::Min(Level, 6);
	
	FString HeaderText = Line.Mid(Level).TrimStartAndEnd();
	return FString::Printf(TEXT("<%s>%s</%s>"), *FString::Printf(TEXT("h%d"), Level), *HeaderText, *FString::Printf(TEXT("h%d"), Level));
}

FString UMounteaDocumentationSystemStatics::ProcessListBlock(const TArray<FString>& Lines, const int32 Start, const int32 End)
{
	bool IsOrdered = IsOrderedListItem(Lines[Start].TrimStartAndEnd());
	FString Result = IsOrdered ? TEXT("<ol>\n") : TEXT("<ul>\n");
	
	for (int32 i = Start; i < End; i++)
	{
		FString ItemText;
		
		if (IsOrdered)
		{
			int32 DotPos = Lines[i].Find(TEXT("."));
			if (DotPos != INDEX_NONE)
			{
				ItemText = Lines[i].Mid(DotPos + 1).TrimStartAndEnd();
			}
		}
		else
		{
			FString Line = Lines[i].TrimStartAndEnd();
			if (Line.StartsWith(TEXT("-")))
			{
				ItemText = Line.Mid(1).TrimStartAndEnd();
			}
			else if (Line.StartsWith(TEXT("*")))
			{
				ItemText = Line.Mid(1).TrimStartAndEnd();
			}
		}
		
		ItemText = ProcessInlineTextForItem(ItemText);
		
		Result += FString::Printf(TEXT("  <li>%s</li>\n"), *ItemText);
	}
	
	Result += IsOrdered ? TEXT("</ol>") : TEXT("</ul>");
	return Result;
}

FString UMounteaDocumentationSystemStatics::ProcessInlineTextForItem(const FString& Text)
{
	FString Result = Text;

	Result = ReplacePattern(Result, TEXT("\\*\\*([^\\*]+)\\*\\*"), TEXT("<strong>$1</strong>"));
	Result = ReplacePattern(Result, TEXT("__([^_]+)__"), TEXT("<strong>$1</strong>"));

	Result = ReplacePattern(Result, TEXT("\\*([^\\*]+)\\*"), TEXT("<em>$1</em>"));
	Result = ReplacePattern(Result, TEXT("_([^_]+)_"), TEXT("<em>$1</em>"));

	Result = ReplacePattern(Result, TEXT("`([^`]+)`"), TEXT("<code>$1</code>"));
	
	return Result;
}

FString UMounteaDocumentationSystemStatics::ProcessBlockquoteLine(const FString& Line)
{
	FString QuoteText = Line.TrimStartAndEnd().Mid(1).TrimStartAndEnd();
	return FString::Printf(TEXT("<blockquote>%s</blockquote>"), *QuoteText);
}

bool UMounteaDocumentationSystemStatics::IsOrderedListItem(const FString& Line)
{
	static const FRegexPattern Pattern(TEXT("^\\s*\\d+\\.\\s+.*$"));
	FRegexMatcher Matcher(Pattern, Line);
	return Matcher.FindNext();
}
