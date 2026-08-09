// Copyright Fillipe Romero. All Rights Reserved.


#include "UI/Eliminations/SP_SpecialEliminationWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USP_SpecialEliminationWidget::InitializeWidget(const FString& ElimMessage, UTexture2D* ElimTexture)
{
	if (IsValid(Text_ElimMessage))
	{
		Text_ElimMessage->SetText(FText::FromString(ElimMessage));
	}
	
	if (IsValid(Image_ElimType))
	{
		Image_ElimType->SetBrushFromTexture(ElimTexture);
	}
}

void USP_SpecialEliminationWidget::CenterWidget(UUserWidget* Widget, float VerticalRatio)
{
	if (!IsValid(Widget)) return;

	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(Widget);
	const float VerticalFraction = VerticalRatio == 0.f ? 1.f : VerticalRatio * 2.f;
	FVector2D CenterPosition(ViewportSize.X / 2.f, VerticalFraction * ViewportSize.Y / 2.f);
	
	Widget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f)); // Align widget center to the center of the viewport
	Widget->SetPositionInViewport(CenterPosition, true);
}
