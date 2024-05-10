// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgets/TestUserWidget.h"

#include "Components/Image.h"

void UTestUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderTranslation(FVector2D(100.0f, 0)); // 设置位置
	SetRenderShear(FVector2D(0, 5)); // 设置倾斜
	
	//SetRenderScale(FVector2D(1, 1.5f)); // 设置缩放

	
	TSubclassOf<UUserWidget> UMGWidgetClass;
	UUserWidget* UMGWidget = CreateWidget<UUserWidget>(GetWorld(), UMGWidgetClass);
 
	// 获取UMG中的Image控件，假设你已经在Blueprint中设置了一个名为"MyImage"的Image控件。
	UImage* Image = Cast<UImage>(UMGWidget->GetWidgetFromName(FName("MyImage")));
 
	// 设置Image的旋转，让它指向Z轴正方向。
	Image->SetRenderTransform(FScaleRotationTranslationMatrix(
		FVector(1.0f, 1.0f, 1.0f), // Scale
		FRotator(0.0f, 0.0f, 90.0f), // Rotation, 指向Z轴正方向
		FVector(0.0f, 0.0f, 0.0f) // Translation
	));
}
