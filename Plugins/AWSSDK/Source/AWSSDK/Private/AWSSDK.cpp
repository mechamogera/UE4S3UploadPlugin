// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AWSSDK.h"
#include "aws/core/Aws.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FAWSSDKModule"

void FAWSSDKModule::StartupModule()
{
#if PLATFORM_WINDOWS
	TArray<FString> DllList;
	DllList.Add(TEXT("aws-checksums.dll"));
	DllList.Add(TEXT("aws-c-common.dll"));
	DllList.Add(TEXT("aws-c-event-stream.dll"));
	DllList.Add(TEXT("aws-cpp-sdk-core.dll"));
	DllList.Add(TEXT("aws-cpp-sdk-s3.dll"));
	const FString BaseDir = IPluginManager::Get().FindPlugin("AWSSDK")->GetBaseDir();
	const FString LibDir = FPaths::Combine(*BaseDir, TEXT("Binaries"), TEXT("Win64"));
	for (auto& Dll : DllList)
	{
		Handles.Add(FPlatformProcess::GetDllHandle(*FPaths::Combine(*LibDir, *Dll)));
	}
#endif

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	Aws::SDKOptions options;
	Aws::InitAPI(options);
}

void FAWSSDKModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	Aws::SDKOptions options;
	Aws::ShutdownAPI(options);

#if PLATFORM_WINDOWS
	for (void* Handle : Handles)
	{
		FPlatformProcess::FreeDllHandle(Handle);
	}
	Handles.Empty();
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAWSSDKModule, AWSSDK)