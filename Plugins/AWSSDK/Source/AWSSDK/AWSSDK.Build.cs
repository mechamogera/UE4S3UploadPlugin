// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class AWSSDK : ModuleRules
{
	public AWSSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        string AWSSDKPath = Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "awssdk");

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
                Path.Combine(AWSSDKPath, "include"),
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Projects",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string libPath = Path.Combine(AWSSDKPath, "lib");
            PublicAdditionalLibraries.AddRange(
            new string[]
            {
                Path.Combine(libPath, "aws-cpp-sdk-core.lib"),
                Path.Combine(libPath, "aws-cpp-sdk-s3.lib"),
            }
            );

            string dllPath = Path.Combine(ModuleDirectory, "..", "..", "Binaries", "Win64");

            string[] dlls = new string[]
            {
                "aws-checksums.dll",
                "aws-c-common.dll",
                "aws-c-event-stream.dll",
                "aws-cpp-sdk-core.dll",
                "aws-cpp-sdk-s3.dll",
            };

            PublicDelayLoadDLLs.AddRange(dlls);

            foreach (string dll in dlls)
            {
                RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(dllPath, dll)));
            }
        }

        string AwsAccessKeyId = System.Environment.GetEnvironmentVariable("AWS_ACCESS_KEY_ID");
        string AwsSecretAccessKey = System.Environment.GetEnvironmentVariable("AWS_SECRET_ACCESS_KEY");
        Definitions.Add(string.Format("AWS_ACCESS_KEY_ID=\"{0}\"", AwsAccessKeyId));
        Definitions.Add(string.Format("AWS_SECRET_ACCESS_KEY=\"{0}\"", AwsSecretAccessKey));
    }
}
