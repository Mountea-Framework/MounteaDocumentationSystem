// All rights reserved Dominik Morse 2024


#include "Statics/MounteaDocumentationSystemStatics.h"

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
