// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "aws/core/Aws.h"
#include "aws/s3/S3Client.h"
#include "S3UploadProxy.generated.h"

/**
 * 
 */
UCLASS()
class AWSSDK_API US3UploadProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCompleteUpload, bool, IsSuccess, FString, Url);
	UPROPERTY(BlueprintAssignable)
	FCompleteUpload OnComplete;

public:
	UFUNCTION(BlueprintCallable, Category="AWSSDK", meta = (BlueprintInternalUseOnly = "true"))
	static US3UploadProxy* UploadTexture(UTexture2D* Texture);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;

private:
	bool PutBinIntoS3ObjectAsync(const FString& BucketName,
		const FString& ObjectName,
		TArray<uint8>& ImageData,
		const FString& RegionStr);

	void ExprotUTexture2DBase(UTexture2D* Img, TArray<uint8>& ImageData);

private:
	const char* BUCKET_NAME = "hogemoge";
	const char* REGION_NAME = "ap-northeast-1";

	UTexture2D* Texture;
	TFuture<void> AsyncTask;
};
