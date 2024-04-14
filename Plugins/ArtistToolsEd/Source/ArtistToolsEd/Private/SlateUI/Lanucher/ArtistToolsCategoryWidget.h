// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ARTISTTOOLSED_API SArtistToolsCategoryWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArtistToolsCategoryWidget)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
