// Fill out your copyright notice in the Description page of Project Settings.

#include "SMaskedImage.h"
#include "Rendering/DrawElements.h"
#include "Widgets/IToolTip.h"
#include "Rendering/SlateRenderer.h"

void SMaskedImage::Construct(const FArguments& InArgs) {
	Image = FInvalidatableBrushAttribute(InArgs._Image);
	MaskImage = FInvalidatableBrushAttribute(InArgs._MaskImage);
	ColorAndOpacity = InArgs._ColorAndOpacity;
	bFlipForRightToLeftFlowDirection = InArgs._FlipForRightToLeftFlowDirection;
	SetOnMouseButtonDown(InArgs._OnMouseButtonDown);

	bHasBeenCached = false;
}

int32 SMaskedImage::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const {
	const FSlateBrush* ImageBrush = Image.GetImage().Get();
	const FSlateBrush* MaskImageBrush = MaskImage.GetImage().Get();

	if ((ImageBrush != nullptr) && (ImageBrush->DrawAs != ESlateBrushDrawType::NoDrawType)) {		
		if (MaskImageBrush != nullptr) {
			if (!bHasBeenCached) {
			
			}
			else {
			
			}
		}
		else {
			const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
			const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

			const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * ImageBrush->GetTint(InWidgetStyle));

			if (bFlipForRightToLeftFlowDirection && GSlateFlowDirection == EFlowDirection::RightToLeft) {
				const FGeometry FlippedGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(-1, 1)));
				FSlateDrawElement::MakeBox(OutDrawElements, LayerId, FlippedGeometry.ToPaintGeometry(), ImageBrush, DrawEffects, FinalColorAndOpacity);
			}
			else {
				FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), ImageBrush, DrawEffects, FinalColorAndOpacity);
			}
		}
	}

	return LayerId;
}

FVector2D SMaskedImage::ComputeDesiredSize(float) const {
	const FSlateBrush* ImageBrush = Image.Get();
	return ImageBrush ? ImageBrush->ImageSize : FVector2D::ZeroVector;
}

void SMaskedImage::SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity) {
	SetAttribute(ColorAndOpacity, InColorAndOpacity, EInvalidateWidgetReason::Paint);
}

void SMaskedImage::SetColorAndOpacity(FLinearColor InColorAndOpacity) {
	SetColorAndOpacity(TAttribute<FSlateColor>(InColorAndOpacity));
}

void SMaskedImage::SetImage(TAttribute<const FSlateBrush*> InImage) {
	bHasBeenCached = false;
	Image.SetImage(*this, InImage);
}

void SMaskedImage::SetMaskImage(TAttribute<const FSlateBrush*> InImage) {
	bHasBeenCached = false;
	MaskImage.SetImage(*this, InImage);
}