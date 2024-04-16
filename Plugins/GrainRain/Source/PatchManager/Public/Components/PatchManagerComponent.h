// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPlatformFilePak.h"
#include "Components/ActorComponent.h"
#include "PatchManagerComponent.generated.h"
//pak下载完成.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDownloadCompleteDelegate, FString ,PakID,bool, bSucceeded);
//pak挂载完成.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMountCompleteDelegate, FString ,PakID,bool, bSucceeded);
/**
 * 补丁管理组件。
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATCHMANAGER_API UPatchManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPatchManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Patching");
	FString PluginMountPoint="ArtResources";
	
	///资产保存路径.
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Patching");
	FString PaksSavePath = "";
	//是否使用缓存的pak文件
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Patching");
	bool bUseCache = false;
	/*资源CDN地址*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Patching");
	FString BaseCdnUrl = "http://127.0.0.1/paks";


	//存储已经下实在的Pak文件
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Patching");
	TMap<FString,FString> DownloadedPaks;
	//挂在Pak
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Patching");
	TMap<FString,FString> MountedPaks;
	
	TSharedPtr<FPakPlatformFile> PakPlatformFile;
	IPlatformFile* InnerPlatformFile;


	//下载完成
	UPROPERTY(BlueprintAssignable, Category="Patching");
	FDownloadCompleteDelegate OnDownloadCompleteDelegate;
	//挂在完成
	UPROPERTY(BlueprintAssignable, Category="Patching");
	FMountCompleteDelegate OnMountCompleteDelegate;

public:

	UFUNCTION(BlueprintCallable, Category = "Patching")
	void RegisterMountPoint(const FString& InMountPoint);
	
	/**
	 * @brief 下载Pak包
	 * @param PakId PakID or PakName
	 */
	UFUNCTION(BlueprintCallable, Category = "Patching")
	void DownloadRemotePakFile(const FString& InPakId);


	// 在文件块下载进程完成时调用
    void OnDownloadComplete(const FString& PakID,bool bSucceeded) const;
    // ChunkDownloader完成挂载文件块时调用
    void OnMountComplete(const FString& PakID,bool bSucceeded) const;
};
