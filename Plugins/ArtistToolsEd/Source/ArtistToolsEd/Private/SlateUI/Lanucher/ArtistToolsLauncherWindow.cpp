// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtistToolsLauncherWindow.h"

#include "ArtistToolsEdStyle.h"
#include "SlateOptMacros.h"
#include "ToolButton.h"
#include "Widgets/SCanvas.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


#define LOCTEXT_NAMESPACE "ArtistToolsLauncherWidget"






void SArtistToolsLauncherWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[
			GetTitleWidget().ToSharedRef()
			
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSpacer)
			.Size(FVector2D(1,10))
		]
		+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("WelcomeArtistTools", "Welcome to the Artist Tools Launcher!"))
			.Font( FCoreStyle::GetDefaultFontStyle( "Regular", 10 ) )
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSpacer)
			.Size(FVector2D(1,20))
		]
		+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		[

			SNew(SCanvas)
			+SCanvas::Slot()
			.Size(FVector2D(700,200))
			.HAlign(HAlign_Center)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Top)
				[
					GetActorTools().ToSharedRef()
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Fill)
				.Padding(FMargin(0,30,0,0))
				[
					SNew(SImage)
					.DesiredSizeOverride(FVector2D(2,2))
					.ColorAndOpacity(FSlateColor(FColor(255,255,255,255)))
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Top)
				[
					GetActorTools().ToSharedRef()
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Fill)
				.Padding(FMargin(0,30,0,0))
				[
					SNew(SImage)
					.DesiredSizeOverride(FVector2D(2,2))
					.ColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
				
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Top)
				[
					GetActorTools().ToSharedRef()
				]
			]
			
		]
		// Populate the widget
	];
}

TSharedPtr<SWidget> SArtistToolsLauncherWindow::GetTitleWidget()
{
	TSharedPtr<SHorizontalBox> Widget;
	SAssignNew(Widget,SHorizontalBox)
	+SHorizontalBox::Slot()
	.AutoWidth()
	[
		SNew(SImage)
		.DesiredSizeOverride(FVector2D(48,48))
		.Image(FArtistToolsEdStyle::Get().GetBrush("ArtistToolsEd.ArtistToolsLogo"))
		
	]
	+SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.AutoWidth()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ArtistTools", "Artist Tools"))
		.Font( FCoreStyle::GetDefaultFontStyle( "Bold", 16 ) )
		//.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
	];

	return Widget;
}

TSharedPtr<SWidget> SArtistToolsLauncherWindow::GetActorTools()
{
	TSharedPtr<SVerticalBox> Widget;
	SAssignNew(Widget,SVerticalBox)
	+SVerticalBox::Slot()
	.HAlign(HAlign_Center)
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("Category","Category"))
		.Font( FCoreStyle::GetDefaultFontStyle( "Bold", 10 ) )
		
	]
	+SVerticalBox::Slot()
	.VAlign(VAlign_Top)
	.Padding(FMargin(20,10,20,10))
	.AutoHeight()
	[
		SNew(SToolButton)
	]
	+SVerticalBox::Slot()
	.VAlign(VAlign_Top)
	.Padding(FMargin(20,10,20,10))
	.AutoHeight()
	[
		SNew(SToolButton)
	]
	+SVerticalBox::Slot()
	.VAlign(VAlign_Top)
	.Padding(FMargin(20,10,20,10))
	.AutoHeight()
	[
		SNew(SToolButton)
	];

	return Widget;
}

TSharedPtr<SWidget> SArtistToolsLauncherWindow::GetAssetTools()
{
	TSharedPtr<SVerticalBox> Widget;
	SAssignNew(Widget,SVerticalBox)
	+SVerticalBox::Slot()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("category","category"))
		
	]
	+SVerticalBox::Slot()
	[
		SNew(SToolButton)
	]
	+SVerticalBox::Slot()
	[
		SNew(SToolButton)
	]
	+SVerticalBox::Slot()
	[
		SNew(SToolButton)
	];

	return Widget;
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
