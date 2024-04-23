// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPlatformFilePak.h"
#include "GameFramework/Actor.h"
#include "LoadPakActor.generated.h"

UCLASS()
class PATCHMANAGER_API ALoadPakActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALoadPakActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	TSharedPtr<FPakPlatformFile> PakPlatformFile;
	IPlatformFile* InnerPlatformFile;

public:
	UFUNCTION(BlueprintCallable,Category=Test)
	bool LoadPak(const FString& InPakPath);
};
