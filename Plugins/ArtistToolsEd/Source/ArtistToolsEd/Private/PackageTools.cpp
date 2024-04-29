#include "PackageTools.h"

#include "ArtistToolsSettings.h"
#include "Settings/PlatformsMenuSettings.h"
#include "Settings/ProjectPackagingSettings.h"
#include "ITurnkeyIOModule.h"
#include "IUATHelperModule.h"
#include "PlatformInfo.h"
#include "AnalyticsEventAttribute.h"
#include "DesktopPlatformModule.h"
#include "FileHelpers.h"
#include "GameProjectGenerationModule.h"

#include "DerivedDataCacheInterface.h"
#include "Interfaces/IProjectTargetPlatformEditorModule.h"
#include "IDesktopPlatform.h"
#include "InstalledPlatformInfo.h"



#define LOCTEXT_NAMESPACE "PackageTools"
namespace 
{
	FCriticalSection GTurnkeySection;
}




void FPackageTools::OnPackageProject()
{
	FName IniPlatformName ="Windows";
	
	TArray<FAnalyticsEventAttribute> AnalyticsParamArray;

	// get a in-memory defaults which will have the user-settings, like the per-platform config/target platform stuff
	const UProjectPackagingSettings* PackagingSettings = GetPackagingSettingsForPlatform(IniPlatformName);
	UPlatformsMenuSettings* PlatformsSettings = GetMutableDefault<UPlatformsMenuSettings>();

	// installed builds only support standard Game type builds (not Client, Server, etc) so instead of looking up a setting that the user can't set, 
	// always use the base PlatformInfo for Game builds, which will be named the same as the platform itself
	const PlatformInfo::FTargetPlatformInfo* PlatformInfo = nullptr;
	if(FApp::IsInstalled())
	{
		PlatformInfo = PlatformInfo::FindPlatformInfo(IniPlatformName);
	}
	else
	{
		PlatformInfo = PlatformInfo::FindPlatformInfo(PlatformsSettings->GetTargetFlavorForPlatform(IniPlatformName));
	}
	// this is unexpected to be able to happen, but it could if there was a bad value saved in the UProjectPackagingSettings - if this trips, we should handle errors
	check(PlatformInfo != nullptr);

	const FString UBTPlatformString = PlatformInfo->DataDrivenPlatformInfo->UBTPlatformString;
	const FString ProjectPath = GetProjectPathForTurnkey();

	// check that we can proceed
	{
		if (FInstalledPlatformInfo::Get().IsPlatformMissingRequiredFile(UBTPlatformString))
		{
			if (!FInstalledPlatformInfo::OpenInstallerOptions())
			{
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MissingPlatformFilesCook", "Missing required files to cook for this platform."));
			}
			return;
		}

		if (!CheckSupportedPlatforms(IniPlatformName))
		{
			return;
		}

	}



	FEditorFileUtils::SaveDirtyPackages(false /*bPromptUserToSave*/
			, true /*bSaveMapPackages*/
			, true /*bSaveContentPackages*/
			, false /*bFastSave*/
			, false /*bNotifyNoPackagesSaved*/
			, false /*bCanBeDeclined*/);
		// force a save of dirty packages before proceeding to run UAT
		// this may delete UProjectPackagingSettings , don't hold it across this call
	
	// basic BuildCookRun params we always want
	FString BuildCookRunParams = FString::Printf(TEXT("-nop4 -utf8output %s -cook "), TEXT("-nocompileeditor -skipbuildeditor"));


	// set locations to engine and project
	if (!ProjectPath.IsEmpty())
	{
		BuildCookRunParams += FString::Printf(TEXT(" -project=\"%s\""), *ProjectPath);
	}

	bool bIsProjectBuildTarget = false;
	const FTargetInfo* BuildTargetInfo = PlatformsSettings->GetBuildTargetInfoForPlatform(IniPlatformName, bIsProjectBuildTarget);

	// Only add the -Target=... argument for code projects. Content projects will return UnrealGame/UnrealClient/UnrealServer here, but
	// may need a temporary target generated to enable/disable plugins. Specifying -Target in these cases will cause packaging to fail,
	// since it'll have a different name.
	if (BuildTargetInfo && bIsProjectBuildTarget)
	{
		BuildCookRunParams += FString::Printf(TEXT(" -target=%s"), *BuildTargetInfo->Name);
	}

	// let the editor add options (-unrealexe in particular)
	{
		BuildCookRunParams += FString::Printf(TEXT(" %s"), *(FString::Printf(TEXT(" -unrealexe=\"%s\""), *FUnrealEdMisc::Get().GetExecutableForCommandlets())));
	}

	// set the platform we are preparing content for
	{
		BuildCookRunParams += FString::Printf(TEXT(" -platform=%s"), *UBTPlatformString);
	}

	// Append any extra UAT flags specified for this platform flavor
	if (!PlatformInfo->UATCommandLine.IsEmpty())
	{
		BuildCookRunParams += FString::Printf(TEXT(" %s"), *PlatformInfo->UATCommandLine);
	}

	// optional settings
	if (PackagingSettings->bSkipEditorContent)
	{
		BuildCookRunParams += TEXT(" -SkipCookingEditorContent");
	}
	if (FDerivedDataCacheInterface* DDC = TryGetDerivedDataCache())
	{
		const TCHAR* GraphName = DDC->GetGraphName();
		if (FCString::Strcmp(GraphName, DDC->GetDefaultGraphName()))
		{
			BuildCookRunParams += FString::Printf(TEXT(" -DDC=%s"), DDC->GetGraphName());
		}
	}
	if (FApp::IsEngineInstalled())
	{
		BuildCookRunParams += TEXT(" -installed");
	}

	if (PackagingSettings->bUseZenStore)
	{
		BuildCookRunParams += TEXT(" -zenstore");
	}




	
	// gather analytics
	const ITargetPlatform* TargetPlatform = GetTargetPlatformManager()->FindTargetPlatform(PlatformInfo->Name);
	TargetPlatform->GetPlatformSpecificProjectAnalytics( AnalyticsParamArray );

	// per mode settings
	FText ContentPrepDescription;
	FText ContentPrepTaskName;
	const FSlateBrush* ContentPrepIcon = nullptr;
	ContentPrepDescription = LOCTEXT("PackagingProjectTaskName", "Packaging project");
	ContentPrepTaskName = LOCTEXT("PackagingTaskName", "Packaging");
	ContentPrepIcon = FAppStyle::Get().GetBrush(TEXT("MainFrame.PackageProject"));

	// let the user pick a target directory
	if (PlatformsSettings->StagingDirectory.Path.IsEmpty())
	{
		PlatformsSettings->StagingDirectory.Path = FPaths::ProjectDir();
	}

	

	const UArtistToolsSettings* settings = GetDefault<UArtistToolsSettings>();

	
	FString OutFolderName = settings->BuildPath.Path;

	//如果指定的打包路径不存在,重新指定...
	if (OutFolderName.IsEmpty() || !FPaths::DirectoryExists(OutFolderName))
	{
		if (!FDesktopPlatformModule::Get()->OpenDirectoryDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr), LOCTEXT("PackageDirectoryDialogTitle", "Package project...").ToString(), PlatformsSettings->StagingDirectory.Path, OutFolderName))
		{
			return;
		}
	}
	

	PlatformsSettings->StagingDirectory.Path = OutFolderName;
	PlatformsSettings->SaveConfig();


	BuildCookRunParams += TEXT(" -stage -archive -package");

	if (ShouldBuildProject(PackagingSettings, TargetPlatform))
	{
		BuildCookRunParams += TEXT(" -build");
	}

	if (PackagingSettings->FullRebuild)
	{
		BuildCookRunParams += TEXT(" -clean");
	}

	// Pak file(s) must be used when using container file(s)
	if (PackagingSettings->UsePakFile || PackagingSettings->bUseIoStore)
	{
		BuildCookRunParams += TEXT(" -pak");
		if (PackagingSettings->bUseIoStore)
		{
			BuildCookRunParams += TEXT(" -iostore");
		}

		if (PackagingSettings->bCompressed)
		{
			BuildCookRunParams += TEXT(" -compressed");
		}
	}

	if (PackagingSettings->IncludePrerequisites)
	{
		BuildCookRunParams += TEXT(" -prereqs");
	}

	if (!PackagingSettings->ApplocalPrerequisitesDirectory.Path.IsEmpty())
	{
		BuildCookRunParams += FString::Printf(TEXT(" -applocaldirectory=\"%s\""), *(PackagingSettings->ApplocalPrerequisitesDirectory.Path));
	}
	else if (PackagingSettings->IncludeAppLocalPrerequisites)
	{
		BuildCookRunParams += TEXT(" -applocaldirectory=\"$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies\"");
	}

	BuildCookRunParams += FString::Printf(TEXT(" -archivedirectory=\"%s\""), *PlatformsSettings->StagingDirectory.Path);

	if (PackagingSettings->ForDistribution)
	{
		BuildCookRunParams += TEXT(" -distribution");
	}

	if (PackagingSettings->bGenerateChunks)
	{
		BuildCookRunParams += TEXT(" -manifests");
	}

	// Whether to include the crash reporter.
	if (PackagingSettings->IncludeCrashReporter && PlatformInfo->DataDrivenPlatformInfo->bCanUseCrashReporter)
	{
		BuildCookRunParams += TEXT(" -CrashReporter");
	}

	if (PackagingSettings->bBuildHttpChunkInstallData)
	{
		BuildCookRunParams += FString::Printf(TEXT(" -manifests -createchunkinstall -chunkinstalldirectory=\"%s\" -chunkinstallversion=%s"), *(PackagingSettings->HttpChunkInstallDataDirectory.Path), *(PackagingSettings->HttpChunkInstallDataVersion));
	}

	EProjectPackagingBuildConfigurations BuildConfig = PlatformsSettings->GetBuildConfigurationForPlatform(IniPlatformName);
	// if PPBC_MAX is set, then the project default should be used instead of the per platform build config
	if (BuildConfig == EProjectPackagingBuildConfigurations::PPBC_MAX)
	{
		BuildConfig = PackagingSettings->BuildConfiguration;
	}

	// when distribution is set, always package in shipping, which overrides the per platform build config
	if (PackagingSettings->ForDistribution)
	{
		BuildConfig = EProjectPackagingBuildConfigurations::PPBC_Shipping;
	}

	const UProjectPackagingSettings::FConfigurationInfo& ConfigurationInfo = UProjectPackagingSettings::ConfigurationInfo[(int)BuildConfig];
	if (BuildTargetInfo)
	{
		if (BuildTargetInfo->Type == EBuildTargetType::Client)
		{
			BuildCookRunParams += FString::Printf(TEXT(" -client -clientconfig=%s"), LexToString(ConfigurationInfo.Configuration));
		}
		else if (BuildTargetInfo->Type == EBuildTargetType::Server)
		{
			BuildCookRunParams += FString::Printf(TEXT(" -server -noclient -serverconfig=%s"), LexToString(ConfigurationInfo.Configuration));
		}
		else
		{
			BuildCookRunParams += FString::Printf(TEXT(" -clientconfig=%s"), LexToString(ConfigurationInfo.Configuration));
		}
	}

	if (ConfigurationInfo.Configuration == EBuildConfiguration::Shipping && !PackagingSettings->IncludeDebugFiles)
	{
		BuildCookRunParams += TEXT(" -nodebuginfo");
	}

	FString TurnkeyParams = FString::Printf(TEXT("-command=VerifySdk -platform=%s -UpdateIfNeeded %s"), *UBTPlatformString, *ITurnkeyIOModule::Get().GetUATParams());
	if (!ProjectPath.IsEmpty())
	{
		TurnkeyParams.Appendf(TEXT(" -project=\"%s\""), *ProjectPath);
	}

	FString CommandLine;
	if (!ProjectPath.IsEmpty())
	{
		CommandLine.Appendf(TEXT(" -ScriptsForProject=\"%s\" "), *ProjectPath);
	}
	CommandLine.Appendf(TEXT("Turnkey %s BuildCookRun %s"), *TurnkeyParams, *BuildCookRunParams);

	RunUAT(CommandLine, PlatformInfo->DisplayName, ContentPrepDescription, ContentPrepTaskName, ContentPrepIcon, &AnalyticsParamArray);
	
	
}


bool FPackageTools::CheckSupportedPlatforms(FName IniPlatformName)
{
#if WITH_EDITOR
	return FModuleManager::LoadModuleChecked<IProjectTargetPlatformEditorModule>("ProjectTargetPlatformEditor").ShowUnsupportedTargetWarning(IniPlatformName);
#endif
}



bool FPackageTools::ShouldBuildProject(const UProjectPackagingSettings* PackagingSettings, const ITargetPlatform* TargetPlatform)
{
	const UProjectPackagingSettings::FConfigurationInfo& ConfigurationInfo = UProjectPackagingSettings::ConfigurationInfo[(int)PackagingSettings->BuildConfiguration];

	// Get the target to build
	const FTargetInfo* Target = PackagingSettings->GetBuildTargetInfo();

	// Only build if the user elects to do so
	bool bBuild = false;
	if (PackagingSettings->Build == EProjectPackagingBuild::Always)
	{
		bBuild = true;
	}
	else if (PackagingSettings->Build == EProjectPackagingBuild::Never)
	{
		bBuild = false;
	}
	else if (PackagingSettings->Build == EProjectPackagingBuild::IfProjectHasCode)
	{
		bBuild = true;
		if (FApp::GetEngineIsPromotedBuild())
		{
			FString BaseDir;

			// Get the target name
			FString TargetName;
			if (Target == nullptr)
			{
				TargetName = TEXT("UnrealGame");
			}
			else
			{
				TargetName = Target->Name;
			}

			// Get the directory containing the receipt for this target, depending on whether the project needs to be built or not
			FString ProjectDir = FPaths::GetPath(FPaths::GetProjectFilePath());
			if (Target != nullptr && FPaths::IsUnderDirectory(Target->Path, ProjectDir))
			{
				UE_LOG(LogTemp, Log, TEXT("Selected target: %s"), *Target->Name);
				BaseDir = ProjectDir;
			}
			else
			{
				FText Reason;

				if (TargetPlatform->RequiresTempTarget(DoesProjectHaveCode(), ConfigurationInfo.Configuration, false, Reason))
				{
					UE_LOG(LogTemp, Log, TEXT("Project requires temp target (%s)"), *Reason.ToString());
					BaseDir = ProjectDir;
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Project does not require temp target"));
					BaseDir = FPaths::EngineDir();
				}
			}

			// Check if the receipt is for a matching promoted target
			FString UBTPlatformName = TargetPlatform->GetTargetPlatformInfo().DataDrivenPlatformInfo->UBTPlatformString;

			extern LAUNCHERSERVICES_API bool  HasPromotedTarget(const TCHAR * BaseDir, const TCHAR * TargetName, const TCHAR * Platform, EBuildConfiguration Configuration, const TCHAR * Architecture);
			if (HasPromotedTarget(*BaseDir, *TargetName, *UBTPlatformName, ConfigurationInfo.Configuration, nullptr))
			{
				bBuild = false;
			}
		}
	}
	else if (PackagingSettings->Build == EProjectPackagingBuild::IfEditorWasBuiltLocally)
	{
		bBuild = !FApp::GetEngineIsPromotedBuild();
	}

	return bBuild;
}

FString FPackageTools::GetProjectPathForTurnkey()
{
	if (FPaths::IsProjectFilePathSet())
	{
		return FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
	}
	if (FApp::HasProjectName())
	{
		FString ProjectPath = FPaths::ProjectDir() / FApp::GetProjectName() + TEXT(".uproject");
		if (FPaths::FileExists(ProjectPath))
		{
			return ProjectPath;
		}
		ProjectPath = FPaths::RootDir() / FApp::GetProjectName() / FApp::GetProjectName() + TEXT(".uproject");
		if (FPaths::FileExists(ProjectPath))
		{
			return ProjectPath;
		}
	}
	return FString();
}

UProjectPackagingSettings* FPackageTools::GetPackagingSettingsForPlatform(FName IniPlatformName)
{
	FString PlatformString = IniPlatformName.ToString();
	UProjectPackagingSettings* PackagingSettings = nullptr;
	for (TObjectIterator<UProjectPackagingSettings> Itr; Itr; ++Itr)
	{
		if (Itr->GetConfigPlatform() == PlatformString)
		{
			PackagingSettings = *Itr;
			break;
		}
	}
	if (PackagingSettings == nullptr)
	{
		PackagingSettings = NewObject<UProjectPackagingSettings>(GetTransientPackage());
		// Prevent object from being GCed.
		PackagingSettings->AddToRoot();
		// make sure any changes to DefaultGame are updated in this class
		PackagingSettings->LoadSettingsForPlatform(PlatformString);
	}

	return PackagingSettings;
}
FString FPackageTools::GetLogAndReportCommandline(FString& LogFilename, FString& ReportFilename)
{
	static int ReportIndex = 0;

	LogFilename = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectIntermediateDir(), *FString::Printf(TEXT("TurnkeyLog_%d.log"), ReportIndex)));
	ReportFilename = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectIntermediateDir(), *FString::Printf(TEXT("TurnkeyReport_%d.log"), ReportIndex++)));

	return FString::Printf(TEXT("-ReportFilename=\"%s\" -log=\"%s\""), *ReportFilename, *LogFilename);
}

bool FPackageTools::DoesProjectHaveCode()
{
	
#if WITH_EDITOR
	FGameProjectGenerationModule& GameProjectModule = FModuleManager::LoadModuleChecked<FGameProjectGenerationModule>(TEXT("GameProjectGeneration"));
	return GameProjectModule.Get().ProjectHasCodeFiles();
#else
	unimplemented();
	return false;
#endif
}

void FPackageTools::ClearDeviceStatus(FName PlatformName)
{
	
	
	FScopeLock Lock(&GTurnkeySection);

	/*FString Prefix = ConvertToDDPIPlatform(PlatformName.ToString()) + "@";
	for (auto& Pair : PerDeviceSdkInfo)
	{
		if (PlatformName == NAME_None || Pair.Key.StartsWith(Prefix))
		{
			Pair.Value.Status = ETurnkeyPlatformSdkStatus::Unknown;
		}
	}
	*/
	
}

void FPackageTools::RunUAT(const FString& CommandLine, const FText& PlatformDisplayName, const FText& TaskName, const FText& TaskShortName, const FSlateBrush* TaskIcon, const TArray<FAnalyticsEventAttribute>* OptionalAnalyticsParamArray, TFunction<void(FString, double)> ResultCallback)
{
#if WITH_EDITOR
	IUATHelperModule::Get().CreateUatTask(CommandLine, PlatformDisplayName, TaskName, TaskShortName, TaskIcon, OptionalAnalyticsParamArray, ResultCallback);
#endif
}

FString FPackageTools::ConvertToDDPIPlatform(const FString& Platform)
{
	FString New = Platform.Replace(TEXT("Editor"), TEXT("")).Replace(TEXT("Client"), TEXT("")).Replace(TEXT("Server"), TEXT(""));
	if (New == TEXT("Win64"))
	{
		New = TEXT("Windows");
	}
	return New;
}


#undef LOCTEXT_NAMESPACE