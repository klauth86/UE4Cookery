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

				auto widgetRenderer = new FWidgetRenderer(true);
				auto imageRenderTarget2D = widgetRenderer->DrawWidget(SNew(SImage).Image(ImageBrush), localSize);
				auto imageTexture = imageRenderTarget2D->ConstructTexture2D(nullptr, "imageRT", EObjectFlags::RF_Transient);

				auto widgetRenderer2 = new FWidgetRenderer(true);
				auto maskImageRenderTarget2D = widgetRenderer2->DrawWidget(SNew(SImage).Image(MaskImageBrush), localSize);
				auto maskImageTexture = maskImageRenderTarget2D->ConstructTexture2D(nullptr, "maskImageRT", EObjectFlags::RF_Transient);

				auto sizeX = imageTexture->GetSizeX();
				auto sizeY = imageTexture->GetSizeY();
				auto size = sizeX * sizeY;

				TArray<FColor> maskedData;
				maskedData.SetNum(size);

				FColor* imageMipData = reinterpret_cast<FColor*>(imageTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));
				FColor* maskImageMipData = reinterpret_cast<FColor*>(maskImageTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));
				if (imageMipData && maskImageMipData) {
					for (size_t i = 0; i < size; i++) {
						maskedData[i].R = imageMipData[i].R;
						maskedData[i].G = imageMipData[i].G;
						maskedData[i].B = imageMipData[i].B;
						maskedData[i].A = imageMipData[i].A * maskImageMipData[i].A;
					}
				}
				imageTexture->PlatformData->Mips[0].BulkData.Unlock();
				maskImageTexture->PlatformData->Mips[0].BulkData.Unlock();

				FCreateTexture2DParameters params;
				auto texture = FImageUtils::CreateTexture2D(sizeX, sizeY, maskedData, nullptr, "MyTexture", EObjectFlags::RF_NoFlags, params);
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