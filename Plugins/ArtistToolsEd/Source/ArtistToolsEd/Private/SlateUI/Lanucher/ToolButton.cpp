// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolButton.h"

#include "ArtistToolsEdStyle.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "ToolButton"

void SToolButton::Construct(const FArguments& InArgs)
{
	
	ChildSlot
	[
		SNew(SBorder)
		[
			SNew(SBox)
			.HeightOverride(48)
			.WidthOverride(160)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Bottom)
				.Padding(FMargin(20,0,0,8))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Button", "Button"))
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				.Padding(FMargin(0,0,20,0))
				[
					SNew(SImage)
					.Image(FArtistToolsEdStyle::Get().GetBrush("ArtistToolsEd.File"))
				]
			]
		]
		// Populate the widget
	];
	
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
