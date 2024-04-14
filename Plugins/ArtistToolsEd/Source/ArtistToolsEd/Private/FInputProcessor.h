#pragma once
#include "ArtistToolsSettings.h"
#include "Framework/Application/IInputProcessor.h"

/**
 * 
 */
class FInputProcessor : public IInputProcessor
{
public:

	FInputProcessor(UArtistToolsSettings* InOwner);

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> ICursor) override;
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual const TCHAR* GetDebugName() const override { return TEXT("ArtistToolsSlateInputProcessor"); }
private:
	
	UArtistToolsSettings* Owner;
};
