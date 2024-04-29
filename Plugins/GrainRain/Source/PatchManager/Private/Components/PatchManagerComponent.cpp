// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PatchManagerComponent.h"

#include "HttpModule.h"
#include "IPlatformFilePak.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"
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
	UKismetSystemLibrary::ExecuteConsoleCommand(this, TEXT("pakcache.Enable 0"));
	// ...

	// 创建Pak文件加载器
	//获取当前使用的平台_
	InnerPlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
	UE_LOG(LogTemp, Warning, TEXT("InnerPlatformFile: %s"), InnerPlatformFile->GetName());
	//初始化PakPlatformFile 
	PakPlatformFile = MakeShareable(new FPakPlatformFile());
	PakPlatformFile.Get()->Initialize(InnerPlatformFile, TEXT(""));// 设置空字符串路径以使用默认的Pak搜索路径
	//RegisterMountPoint(PluginMountPoint);
	PaksSavePath = FPaths::Combine(FPaths::ProjectPersistentDownloadDir(),"/Game","Paks");

}

void UPatchManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	
	UnmountAllPakFiles();
	
	
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

TArray<FString> UPatchManagerComponent::MountPakFile(const FString& InPakId)
{

	const FString* PakFilePath = DownloadedPaks.Find(InPakId);
	TArray<FString> OutMountedPaths;

	
	if(PakFilePath==nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[MountRemotePakFile]Pak not downloaded: %s"), *InPakId);
		return OutMountedPaths;
	}
	// 切换到 pak平台
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile.Get());

	// 获取pak文件
	FPakFile* PakFile = new FPakFile(PakPlatformFile.Get(), **PakFilePath, false);

	FString MountPoint(*PakFile->GetMountPoint());

	UE_LOG(LogTemp, Display, TEXT("[MountRemotePakFile]Default Mount Point: %s"), *MountPoint);
	MountPoint = FPaths::Combine(FPaths::ProjectDir(),  "Content");

	auto bMountSuccess=PakPlatformFile->Mount(**PakFilePath, 0,*MountPoint);
	//挂载资产...
	if (bMountSuccess)
	{
		//带上/Mod/作为搜索路径，避免默认从/Game/中搜索
		TArray<FString> AssetList;
		PakFile->FindPrunedFilesAtPath(AssetList, *PakFile->GetMountPoint(), true, false, true);
		for (FString itemPath : AssetList)
		{
			if (itemPath.EndsWith(TEXT(".uasset"))||itemPath.EndsWith(TEXT(".umap")))
			{
				FString NewFileName = itemPath;
				NewFileName.RemoveFromEnd(TEXT(".uasset"));
				NewFileName.RemoveFromEnd(TEXT(".umap"));

				int Pos = NewFileName.Find("/Content/");
				NewFileName = NewFileName.RightChop(Pos + 8);
				NewFileName = "/Game"+NewFileName;
				

				OutMountedPaths.Add(NewFileName);

				auto obj =LoadObject<UObject>(nullptr, *NewFileName);
				if(obj!=nullptr)
				{
					UE_LOG(LogTemp,Display,TEXT("-------Material Name:%s"),*obj->GetName())
				}

				
				if(obj!=nullptr)
				{
					//OutMountedPaths.Add(NewFileName);

					UE_LOG(LogTemp, Display, TEXT("[MountPak]FileName:%s,objname:%s"), *NewFileName,*obj->GetClass()->GetName());
					if(obj->IsA<UStaticMesh>())
					{
						AStaticMeshActor* Mesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),
							FVector(0,0,0), FRotator::ZeroRotator);
						Mesh->SetMobility(EComponentMobility::Type::Movable);
						Mesh->GetStaticMeshComponent()->SetStaticMesh(Cast<UStaticMesh>(obj));
						auto mat = Mesh->GetStaticMeshComponent()->GetMaterial(0);

						if (mat!=nullptr)
						{
							UE_LOG(LogTemp,Display,TEXT("-------Material Name:%s"),*mat->GetName())
						}
					}
					else if(obj->IsA<USkeletalMesh>())
					{
						ASkeletalMeshActor* Mesh = GetWorld()->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(),
							FVector(0,0,0), FRotator::ZeroRotator);
						Mesh->GetSkeletalMeshComponent()->SetSkeletalMesh(Cast<USkeletalMesh>(obj));
						auto mat = Mesh->GetSkeletalMeshComponent()->GetMaterial(0);

						if (mat!=nullptr)
						{
							UE_LOG(LogTemp,Display,TEXT("-------Material Name:%s"),*mat->GetName())
						}
					}
					else if (obj->IsA<UWorld>())
					{
						bool bOutSuccess = false;
						//ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), obj, FVector::Zero(), FRotator::ZeroRotator, bOutSuccess);
						if (bOutSuccess)
						{
							UE_LOG(LogTemp, Warning, TEXT("子关卡加载成功!"));
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("子关卡加载失败!"));
						}

					}
					//UE_LOG(LogTemp, Warning, TEXT("[MountPak]FileName:%s,objname:%s"), *NewFileName,*obj->GetClass()->GetName());
				}
				////else
				//{
					//UE_LOG(LogTemp, Warning, TEXT("[MountPak]FileName:%s,objname is null"), *NewFileName);
				//}
			}
		}
	}

	// 切换到 pak平台
	//FPlatformFileManager::Get().SetPlatformFile(*InnerPlatformFile);
	OnMountComplete(InPakId,bMountSuccess);
	return OutMountedPaths;
	
}




void UPatchManagerComponent::TestLoadPak(FString InPakId)
{
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile.Get());


	const FString* PakFilePath = DownloadedPaks.Find(InPakId);
	TArray<FString> OutMountedPaths;

	
	if(PakFilePath==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MountRemotePakFile]Pak not downloaded: %s"), *InPakId);
		return;
	}
 
	FPakFile* TmpPak = new FPakFile(PakPlatformFile.Get(), **PakFilePath, false);
	FString PakMountPoint = TmpPak->GetMountPoint();
	int32 Pos2 = PakMountPoint.Find("Content/");
	FString NewMountPoint = PakMountPoint.RightChop(Pos2);
	NewMountPoint = FPaths::ProjectDir() + NewMountPoint;
	//FString NewMountPoint = "../../../DBJTest/Content/DLC/";
 
	TmpPak->SetMountPoint(*NewMountPoint);
 
	if (PakPlatformFile->Mount(**PakFilePath, 1, *NewMountPoint))
	{
		TArray<FString> FoundFilenames;
		TmpPak->FindPrunedFilesAtPath(*TmpPak->GetMountPoint(),FoundFilenames , true, false, false);
 
		if (FoundFilenames.Num() > 0)
		{
			for (FString& Filename : FoundFilenames)
			{
				if (Filename.EndsWith(TEXT(".uasset")))
				{
					FString NewFileName = Filename;
					NewFileName.RemoveFromEnd(TEXT(".uasset"));
					int32 Pos = NewFileName.Find("/Content/");
					NewFileName = NewFileName.RightChop(Pos + 8);
					NewFileName = "/Game" + NewFileName;
 
					UObject* LoadedObj = StaticLoadObject(UObject::StaticClass(), NULL, *NewFileName);
 
					UStaticMesh* SM = Cast<UStaticMesh>(LoadedObj);
					if (SM)
					{
						AStaticMeshActor* MeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0,0,460), FRotator(0,0,0) );
						MeshActor->SetMobility(EComponentMobility::Movable);
						MeshActor->GetStaticMeshComponent()->SetStaticMesh(SM);
						break;
					}
				}
			}
		}
	}
 
 
	//FPlatformFileManager::Get().SetPlatformFile(*InnerPlatformFile);
 
}

void UPatchManagerComponent::UnmountAllPakFiles()
{
	// 切换到 pak平台
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile.Get());
	for (auto pair : MountedPaks)
	{
		// 获取pak文件
		FPakFile* PakFile = new FPakFile(InnerPlatformFile, *pair.Key, false);
		FString MountPoint = PakFile->GetMountPoint();

		const auto bUnmountSuccess  = PakPlatformFile->Unmount(*pair.Key);
		if(bUnmountSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unmount Point Full Path: %s"), *pair.Key);
		}else
		{
			UE_LOG(LogTemp, Error, TEXT("Unmount Pak Failed"));
		}
	}
	// 将Pak文件设置为默认文件系统
	FPlatformFileManager::Get().SetPlatformFile(*InnerPlatformFile);
}

void UPatchManagerComponent::OnDownloadComplete(const FString& PakID, const bool bSucceeded) const
{
	OnDownloadCompleteDelegate.Broadcast(PakID,bSucceeded);
}

void UPatchManagerComponent::OnMountComplete(const FString& PakID, const bool bSucceeded) const
{
	OnMountCompleteDelegate.Broadcast(PakID,bSucceeded);
}

