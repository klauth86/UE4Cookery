// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SLeafWidget.h"

class UTexture2D;

class CPP010_API SMaskedImage : public SLeafWidget {

public:

	SLATE_BEGIN_ARGS(SMaskedImage)
		: _Image(FCoreStyle::Get().GetDefaultBrush())
		, _MaskImage(nullptr)
		, _ColorAndOpacity(FLinearColor::White)
		, _FlipForRightToLeftFlowDirection(false) {}

	/** Image resource */
	SLATE_ATTRIBUTE(const FSlateBrush*, Image)
		
	SLATE_ATTRIBUTE(const FSlateBrush*, MaskImage)

	/** Color and opacity */
	SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

	/** Flips the image if the localization's flow direction is RightToLeft */
	SLATE_ARGUMENT(bool, FlipForRightToLeftFlowDirection)

	/** Invoked when the mouse is pressed in the widget. */
	SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)
	
	SLATE_END_ARGS()

	SMaskedImage() {
		SetCanTick(false);
		bCanSupportFocus = false;
	}

	void Construct(const FArguments& InArgs);

public:

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	/** See the Image attribute */
	void SetImage(TAttribute<const FSlateBrush*> InImage);

	/** See the MaskImage attribute */
	void SetMaskImage(TAttribute<const FSlateBrush*> InImage);

public:

	// SWidget overrides
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

protected:
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

protected:

	TWeakObjectPtr<UTexture2D> MaskedTexturePtr;

	FSlateBrush MaskedBrush;

	/** The slate brush to draw for the image that we can invalidate. */
	FInvalidatableBrushAttribute Image;

	/** The slate brush to mask the image that we can invalidate. */
	FInvalidatableBrushAttribute MaskImage;

	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

	/** Flips the image if the localization's flow direction is RightToLeft */
	bool bFlipForRightToLeftFlowDirection;

	/** Invoked when the mouse is pressed in the image */
	FPointerEventHandler OnMouseButtonDownHandler;
};