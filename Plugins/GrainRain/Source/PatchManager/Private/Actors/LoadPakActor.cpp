// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LoadPakActor.h"

#include "IPlatformFilePak.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"


// Sets default values
ALoadPakActor::ALoadPakActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALoadPakActor::BeginPlay()
{
	Super::BeginPlay();
	//获取当前使用的平台
	InnerPlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
	UE_LOG(LogTemp,Warning,TEXT("InnerPlatformFile: %s"),InnerPlatformFile->GetName());
	//初始化PakPlatformFile
	PakPlatformFile = MakeShareable(new FPakPlatformFile());
	PakPlatformFile.Get()->Initialize(InnerPlatformFile,TEXT(""));
}

// Called every frame
void ALoadPakActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ALoadPakActor::LoadPak(const FString& InPakPath)
{

	bool Result = false;
	//切换到Pak平台
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile.Get());

	//获取Pak文件
	FPakFile* PakFile = new FPakFile(InnerPlatformFile,*InPakPath,false);

	FString MountPoint = PakFile->GetMountPoint();
	UE_LOG(LogTemp,Warning,TEXT("Default Mount Point : %s"),*MountPoint);

#if WITH_EDITOR
	// PIE模式下，MountPoint 使用绝对路径
	// 打包模式下，MountPoint 使用相对路径
	MountPoint = FPaths::ConvertRelativePathToFull(MountPoint);
	UE_LOG(LogTemp, Warning, TEXT("Default Mount Point Full Path: %s"), *MountPoint);
	MountPoint = FPaths::ProjectContentDir() + TEXT("DLC/");
	// 可在此处检测 默认MountPoint的绝对路径释放和本条语句执行结果是否一致
	MountPoint = FPaths::ConvertRelativePathToFull(MountPoint);
	PakFile->SetMountPoint(*MountPoint);
#endif

	if (PakPlatformFile->Mount(*InPakPath,1,*MountPoint))
	{
		// 遍历 pak 里的资源
		TArray<FString> AssetList;
		PakFile->FindPrunedFilesAtPath(*InPakPath, AssetList, true, false, true);
		
		for (FString itemPath : AssetList)
		{
			UE_LOG(LogTemp, Warning, TEXT("%-30s\t%s"), *FPackageName::GetShortName(itemPath), *itemPath);
			// 此处可异步加载资源

			if (itemPath.EndsWith(TEXT(".uasset"))||itemPath.EndsWith(TEXT(".umap")))
			{
				FString NewFileName = itemPath;
				NewFileName.RemoveFromEnd(TEXT(".uasset"));
				NewFileName.RemoveFromEnd(TEXT(".umap"));
				//UE_LOG(LogTemp,Display,TEXT("[MountRemotePakFile]OldFileName:%s"),*NewFileName);

				
				int Pos = NewFileName.Find("/Content/");
				NewFileName = NewFileName.RightChop(Pos + 8);
				
				NewFileName = "/Game"+NewFileName;
				
				//NewFileName.ReplaceInline(*MountPoint, *FString::Format(TEXT("/{0}/"),{PluginMountPoint}));
				//UE_LOG(LogTemp,Display,TEXT("[MountPak]NewFileName:%s"),*NewFileName);
				
				auto obj =LoadObject<UObject>(nullptr, *NewFileName);
				if(obj!=nullptr)
				{

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
					}else if (obj->IsA<UWorld>())
					{
						bool bOutSuccess = false;
	
						//ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), obj, FVector::Zero(), FRotator::ZeroRotator, bOutSuccess);
						if (bOutSuccess)
						{
							UE_LOG(LogTemp, Warning, TEXT("子关卡加载成功!"));
						}else
						{
							UE_LOG(LogTemp, Warning, TEXT("子关卡加载失败!"));
						}

							//LevelObjs.Add(obj);
					}
				
					//UE_LOG(LogTemp, Warning, TEXT("[MountPak]FileName:%s,objname:%s"), *NewFileName,*obj->GetClass()->GetName());
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("[MountPak]FileName:%s,objname is null"), *NewFileName);
				}
			}
		}			
	}


	// 设置回原来的PlatformFile, UE4.26 
	// 不加该条语句，本测试崩溃，报错：Pure Virtual function being called while application was running
	FPlatformFileManager::Get().SetPlatformFile(*InnerPlatformFile);
	return Result;
}

