// Fill out your copyright notice in the Description page of Project Settings.


#include "S3UploadProxy.h"
#include "aws/s3/model/PutObjectRequest.h"
#include "aws/core/auth/AWSCredentials.h"
#include <iostream>
#include <fstream>
#include "ImageUtils.h"
#include "Serialization/BufferArchive.h"
#include "GenericPlatform/GenericPlatformProcess.h"

US3UploadProxy::US3UploadProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

US3UploadProxy* US3UploadProxy::UploadTexture(UTexture2D* Texture)
{
	US3UploadProxy* Proxy = NewObject<US3UploadProxy>();
	Proxy->Texture = Texture;
	return Proxy;
}


void US3UploadProxy::Activate()
{
	TArray<uint8> ImageData;
	ExprotUTexture2DBase(Texture, ImageData);

	FString Uuid = FGuid::NewGuid().ToString() + FString(TEXT(".png"));
	UE_LOG(LogTemp, Warning, TEXT("uuid:%s"), *Uuid);

	// Assign these values before running the program
	FString BucketName(BUCKET_NAME);
	FString Region(REGION_NAME);      // Optional

	// Put the file into the S3 bucket
	PutBinIntoS3ObjectAsync(BucketName, Uuid, ImageData, Region);
}

bool US3UploadProxy::PutBinIntoS3ObjectAsync(
	const FString& BucketName,
	const FString& ObjectName,
	TArray<uint8>& ImageData,
	const FString& RegionStr
)
{
	TUniqueFunction<void()> Task = [this, BucketName, ObjectName, ImageData, RegionStr]() {
		const Aws::String& S3BucketName = (const char*)TCHAR_TO_ANSI(*BucketName);
		const Aws::String& S3ObjectName = (const char*)TCHAR_TO_ANSI(*ObjectName);
		const Aws::String& Region = (const char*)TCHAR_TO_ANSI(*RegionStr);

		// If region is specified, use it
		Aws::Client::ClientConfiguration ClientConfig;
		if (!Region.empty())
		{
			ClientConfig.region = Region;
		}


		Aws::Auth::AWSCredentials Credentials(AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY);

		// Set up request
		Aws::S3::S3Client S3Client(Credentials, ClientConfig);
		Aws::S3::Model::PutObjectRequest ObjectRequest;

		ObjectRequest.SetBucket(S3BucketName);
		ObjectRequest.SetKey(S3ObjectName);
		auto Data = Aws::MakeShared<Aws::StringStream>("PutObjectInputStream", std::stringstream::in | std::stringstream::out | std::stringstream::binary);
		Data->write((char*)ImageData.GetData(), ImageData.Num());
		ObjectRequest.SetBody(Data);

		// Put the string into the S3 object
		auto PutObjectOutcome = S3Client.PutObject(ObjectRequest);
		Aws::String Url = S3Client.GeneratePresignedUrl(S3BucketName, S3ObjectName, Aws::Http::HttpMethod::HTTP_GET);
		this->OnComplete.Broadcast(PutObjectOutcome.IsSuccess(), Url.c_str());
	};
	auto Result = Async(EAsyncExecution::ThreadPool, MoveTemp(Task));

	return true;
}

void US3UploadProxy::ExprotUTexture2DBase(UTexture2D* Img, TArray<uint8>& ImgData)
{
	TextureCompressionSettings OldCompressionSettings = Img->CompressionSettings; 
	bool OldSRGB = Img->SRGB;

	Img->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	Img->SRGB = false;
	Img->UpdateResource();

	FTexture2DMipMap & mipmap = Img->PlatformData->Mips[0];
	int texturex = Img->PlatformData->SizeX;
	int texturey = Img->PlatformData->SizeY;
	TArray<FColor> nColors;
	FColor* FormatedImageData = static_cast<FColor*>(Img->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	for (int32 y = 0; y < texturey; y++)
	{
		for (int32 x = 0; x < texturex; x++)
		{
			FColor bColor;
			int32 curPixelIndex = (y *texturex) + x;
			FColor pixel = FormatedImageData[curPixelIndex];
			bColor.B = pixel.R;
			bColor.G = pixel.G;
			bColor.R = pixel.B;
			bColor.A = pixel.A;
			nColors.Add(bColor);
		}
	}
	mipmap.BulkData.Unlock();
	Img->CompressionSettings = OldCompressionSettings;
	Img->SRGB = OldSRGB;

	Img->UpdateResource();

	FImageUtils::CompressImageArray(texturex, texturey, nColors, ImgData);
}