// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PatchManagerComponent.h"

#include "HttpModule.h"
#include "IPlatformFilePak.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UPatchManagerComponent::UPatchManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPatchManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	//启用 Pak 缓存
	UKismetSystemLibrary::ExecuteConsoleCommand(this, TEXT("pakcache.Enable 0"));
	//创建Pak文件加载器
	InnerPlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
	UE_LOG(LogTemp, Warning, TEXT("InnerPlatformFile: %s"), InnerPlatformFile->GetName());
	//初始化PakplatformFile
	PakPlatformFile = MakeShareable(new FPakPlatformFile());
	//设置空字符串路径,以使用默认的Pak搜索路径.
	PakPlatformFile.Get()->Initialize(InnerPlatformFile,TEXT(""));
	PaksSavePath = FPaths::Combine(FPaths::ProjectPersistentDownloadDir(),PluginMountPoint,"Paks");
	RegisterMountPoint(PluginMountPoint);
	



	
	
}

void UPatchManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	
	#if !WITH_EDITOR
	FPackageName::UnRegisterMountPoint(TEXT("/ArtResources/"), FPaths::Combine(FPaths::ProjectPluginsDir(),"ArtResources/Content" ));
	#endif
	
	// 将Pak文件设置为默认文件系统
	FPlatformFileManager::Get().SetPlatformFile(*InnerPlatformFile);
}


// Called every frame
void UPatchManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPatchManagerComponent::RegisterMountPoint(const FString& InMountPoint)
{
	//FPackageName::RegisterMountPoint(TEXT("/ArtResources/"), FPaths::Combine(FPaths::ProjectPluginsDir(),"ArtResources/Content" ));
	FPackageName::RegisterMountPoint(
		*FString::Format(TEXT("/{0}/"),{InMountPoint}),
		FPaths::Combine(FPaths::ProjectPluginsDir(),InMountPoint,"Content")
		);
}
/**
 * @brief 下载Pak包
 * @param PakId PakID or PakName
 */
void UPatchManagerComponent::DownloadRemotePakFile(const FString& InPakId)
{
	const FString path =FPaths::Combine(PaksSavePath,InPakId);

	
	if(PakPlatformFile->FileExists(*path) && bUseCache)
	{
		//直接使用缓存。
		DownloadedPaks.Add(InPakId,path);
		OnDownloadComplete(InPakId,true);
		UE_LOG(LogTemp, Display, TEXT("Pak file Exists: %s"), *path);
		return;
	}
	
	const TSharedPtr<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(FPaths::Combine(BaseCdnUrl,InPakId));
	//HttpRequest->SetHeader("referer","one.heartdub.cn");
	UE_LOG(LogTemp, Display, TEXT("Pak Remote url: %s"), *HttpRequest->GetURL());
	
	HttpRequest->OnProcessRequestComplete().BindLambda([this,path, InPakId](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response->GetResponseCode() == EHttpResponseCodes::Ok)
		{
			TArray<uint8> ResponseData = Response->GetContent();
			//const FString path =FPaths::Combine(PaksSavePath,InPakId);
			// 保存下载的Pak文件到本地
			// 保存下载的Pak文件到本地指定位置.文件用ID命名
			if (FFileHelper::SaveArrayToFile(ResponseData, *path))
			{
				DownloadedPaks.Add(InPakId,path);
				UE_LOG(LogTemp, Display, TEXT("Saved Pak file: %s"), *path);
				OnDownloadComplete(InPakId,true);
				// 下载成功，可以加载Pak文件
				//LoadRemotePakFile(LocalPakPath);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to save Pak file: %s"), *path);
				OnDownloadComplete(InPakId,false);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to download Pak file"));
			OnDownloadComplete(InPakId,false);
		}
	});

	HttpRequest->ProcessRequest();
}

void UPatchManagerComponent::OnDownloadComplete(const FString& PakID, const bool bSucceeded) const
{
	OnDownloadCompleteDelegate.Broadcast(PakID,bSucceeded);
}

void UPatchManagerComponent::OnMountComplete(const FString& PakID, const bool bSucceeded) const
{
	OnMountCompleteDelegate.Broadcast(PakID,bSucceeded);
}

