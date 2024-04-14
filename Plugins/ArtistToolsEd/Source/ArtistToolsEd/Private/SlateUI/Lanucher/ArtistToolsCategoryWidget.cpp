// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtistToolsCategoryWidget.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "ToolButton"
void SArtistToolsCategoryWidget::Construct(const FArguments& InArgs)
{

	struct FButtonData
	{
		FButtonData( const FText& InDisplayName )
			: DisplayName( InDisplayName )
		{

		}

		FText DisplayName;
	};
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("category","category"))
		]
		
		
	];
}
#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
