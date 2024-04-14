// Fill out your copyright notice in the Description page of Project Settings.


#include "AboutWidget.h"

#include "ArtistToolsEdStyle.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAboutWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.FillWidth(0.3f)
		[
			SNew(SBox)
			.WidthOverride(32)
			.HeightOverride(32)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(FArtistToolsEdStyle::Get().GetBrush("ArtistToolsEd.ArtistToolsLogo"))
			]
		]
		+SHorizontalBox::Slot()
		.FillWidth(0.7f)
		[
			SNew(SBox)
			
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(FArtistToolsEdStyle::Get().GetBrush("ArtistToolsEd.ArtistToolsLogo"))
			]
		]
		// Populate the widget
		
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
