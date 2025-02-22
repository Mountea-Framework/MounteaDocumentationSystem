// Copyright Epic Games, Inc. All Rights Reserved.

#include "MounteaDocumentationSystem.h"

#include "Style/MounteaDocumentationStyle.h"

#define LOCTEXT_NAMESPACE "FMounteaDocumentationSystemModule"

void FMounteaDocumentationSystemModule::StartupModule()
{
	// Initialize Style
	{
		FMounteaDocumentationStyle::Initialize();
	}
}

void FMounteaDocumentationSystemModule::ShutdownModule()
{
	// Shutdown Style
	{
		FMounteaDocumentationStyle::Shutdown();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMounteaDocumentationSystemModule, MounteaDocumentationSystem)