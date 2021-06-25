// Fill out your copyright notice in the Description page of Project Settings.

#include "SMaskedImage.h"
#include "Widgets/Images/SImage.h"
#include "ImageUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"

void SMaskedImage::Construct(const FArguments& InArgs) {
	Image = FInvalidatableBrushAttribute(InArgs._Image);
	MaskImage = FInvalidatableBrushAttribute(InArgs._MaskImage);
	ColorAndOpacity = InArgs._ColorAndOpacity;
	bFlipForRightToLeftFlowDirection = InArgs._FlipForRightToLeftFlowDirection;
	SetOnMouseButtonDown(InArgs._OnMouseButtonDown);
	SetCanTick(true);
}

int32 SMaskedImage::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const {
	const FSlateBrush* ImageBrush = Image.GetImage().Get();
	const FSlateBrush* MaskImageBrush = MaskImage.GetImage().Get();

	if ((ImageBrush != nullptr) && (ImageBrush->DrawAs != ESlateBrushDrawType::NoDrawType)) {		
		if (MaskImageBrush != nullptr) {			
			if (MaskedTexturePtr.IsValid()) {
				const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
				const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

				const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * ImageBrush->GetTint(InWidgetStyle));

				if (bFlipForRightToLeftFlowDirection && GSlateFlowDirection == EFlowDirection::RightToLeft) {
					const FGeometry FlippedGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(-1, 1)));
					FSlateDrawElement::MakeBox(OutDrawElements, LayerId, FlippedGeometry.ToPaintGeometry(), &MaskedBrush, DrawEffects, FinalColorAndOpacity);
				}
				else {
					FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), &MaskedBrush, DrawEffects, FinalColorAndOpacity);
				}
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

void SMaskedImage::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	SLeafWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	const FSlateBrush* ImageBrush = Image.GetImage().Get();
	const FSlateBrush* MaskImageBrush = MaskImage.GetImage().Get();

	if ((ImageBrush != nullptr) && (ImageBrush->DrawAs != ESlateBrushDrawType::NoDrawType)) {
		if (MaskImageBrush != nullptr) {
			if (!MaskedTexturePtr.IsValid()) {

				auto localSize = AllottedGeometry.GetLocalSize();

				TArray<FColor> imageData;
				auto widgetRenderer = new FWidgetRenderer(true);
				auto imageRenderTarget2D = widgetRenderer->DrawWidget(SNew(SImage).Image(ImageBrush), localSize);
				imageRenderTarget2D->GameThread_GetRenderTargetResource()->ReadPixels(imageData);

				TArray<FColor> maskImageData;
				auto widgetRenderer2 = new FWidgetRenderer(true);
				auto maskImageRenderTarget2D = widgetRenderer2->DrawWidget(SNew(SImage).Image(MaskImageBrush), localSize);
				maskImageRenderTarget2D->GameThread_GetRenderTargetResource()->ReadPixels(maskImageData);

				auto sizeX = imageRenderTarget2D->SizeX;
				auto sizeY = imageRenderTarget2D->SizeY;
				auto size = sizeX * sizeY;

				TArray<FColor> maskedData;
				maskedData.SetNum(size);

				if (sizeX > 0 && sizeY > 0) {
					for (size_t i = 0; i < size; i++) {
						maskedData[i].R = imageData[i].R;
						maskedData[i].G = imageData[i].G;
						maskedData[i].B = imageData[i].B;
						maskedData[i].A = imageData[i].A * maskImageData[i].A;
					}
				}

				FCreateTexture2DParameters params;
				params.bUseAlpha = true;
				params.bDeferCompression = true;

				auto texture = FImageUtils::CreateTexture2D(sizeX, sizeY, maskedData, GetTransientPackage(), "MyTexture", EObjectFlags::RF_NoFlags, params);
				MaskedBrush = FSlateBrush();
				MaskedBrush.SetResourceObject(texture);

				texture->AddToRoot();
				MaskedTexturePtr = texture;
			}
		}
	}

	SetCanTick(false);
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
	if (MaskedTexturePtr.IsValid()) {
		MaskedTexturePtr.Get()->RemoveFromRoot();
		MaskedTexturePtr.Reset();
	}
	Image.SetImage(*this, InImage);
	SetCanTick(true);
}

void SMaskedImage::SetMaskImage(TAttribute<const FSlateBrush*> InImage) {
	if (MaskedTexturePtr.IsValid()) {
		MaskedTexturePtr.Get()->RemoveFromRoot();
		MaskedTexturePtr.Reset();
	}
	MaskImage.SetImage(*this, InImage);
	SetCanTick(true);
}