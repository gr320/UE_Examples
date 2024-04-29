#pragma once
#include "Settings/ProjectPackagingSettings.h"

class FPackageTools
{
public:
	
	void OnPackageProject();
	bool CheckSupportedPlatforms(FName IniPlatformName);
	bool ShouldBuildProject(const UProjectPackagingSettings* PackagingSettings, const ITargetPlatform* TargetPlatform);
	FString GetProjectPathForTurnkey();
	UProjectPackagingSettings* GetPackagingSettingsForPlatform(FName IniPlatformName);
	FString GetLogAndReportCommandline(FString& LogFilename, FString& ReportFilename);
	bool DoesProjectHaveCode();
	void ClearDeviceStatus(FName PlatformName=NAME_None);
	void RunUAT(const FString& CommandLine, const FText& PlatformDisplayName, const FText& TaskName, const FText& TaskShortName, const FSlateBrush* TaskIcon,  const TArray<struct FAnalyticsEventAttribute>* OptionalAnalyticsParamArray = nullptr, TFunction<void(FString, double)> ResultCallback=TFunction<void(FString, double)>());
	FString ConvertToDDPIPlatform(const FString& Platform);
};