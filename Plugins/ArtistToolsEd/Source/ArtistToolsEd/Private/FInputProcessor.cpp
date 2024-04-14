#include "FInputProcessor.h"

#include "Kismet/KismetInternationalizationLibrary.h"

FInputProcessor::FInputProcessor(UArtistToolsSettings* InOwner)
{
	Owner = InOwner;
}

void FInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> ICursor)
{
}

bool FInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (IsValid(Owner))
	{
		//获取输入按键。
		FInputChord InputChord = FInputChord(InKeyEvent.GetKey(),InKeyEvent.IsShiftDown(),InKeyEvent.IsControlDown(),InKeyEvent.IsAltDown(),InKeyEvent.IsCommandDown());

		//与设置中输入相匹配
		if (Owner->LanguageShortcutKey == InputChord)
		{
			Owner->GetOpenTheBlueprint();
			FString Lang= UKismetInternationalizationLibrary::GetCurrentLanguage();
			Owner->Switch = *Owner->LanguageMap.Find(Owner->Language);
			if (Lang !=Owner->Switch)
			{
				Owner->Previous = UKismetInternationalizationLibrary::GetCurrentLanguage();
				UKismetInternationalizationLibrary::SetCurrentLanguage(Owner->Switch,true);
			}
			else
			{
				UKismetInternationalizationLibrary::SetCurrentLanguage(Owner->Previous,true);
			}
			Owner->RefreshBlueprints();
		}
	}
	return false;
}

bool FInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	return false;
}

bool FInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	return false;
}

bool FInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return false;
}
