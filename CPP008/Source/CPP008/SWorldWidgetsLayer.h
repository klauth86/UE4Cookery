// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Layout/Visibility.h"
#include "UObject/ObjectKey.h"

class UObject;
class APlayerController;
class AActor;

class CPP008_API FComponentEntry {

public:
	FComponentEntry() :Slot(nullptr) { bRemoving = false; bDrawAtDesiredSize = false; }
	~FComponentEntry() {
		Widget.Reset();
		ContainerWidget.Reset();
	}

public:

	TWeakObjectPtr<UObject> OwningObject;

	TSharedPtr<SWidget> ContainerWidget;
	TSharedPtr<SWidget> Widget;
	SConstraintCanvas::FSlot* Slot;
	FIntPoint DrawSize;
	FVector2D Pivot;
	FVector WorldLocation;
	TWeakObjectPtr<AActor> Actor;

	uint8 bRemoving : 1;
	uint8 bDrawAtDesiredSize : 1;
};

class CPP008_API SWorldWidgetsLayer : public SCompoundWidget {

	SLATE_BEGIN_ARGS(SWorldWidgetsLayer) {
		_Visibility = EVisibility::SelfHitTestInvisible;
	}
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs, APlayerController* playerController);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }

	void AddComponent(UObject* owningObject, const FComponentEntry& entry);

	void RemoveComponent(UObject* owningObject);

protected:

	void RemoveEntryFromCanvas(FComponentEntry& Entry);

private:

	TWeakObjectPtr<APlayerController> PlayerControllerPtr;

	TMap<FObjectKey, FComponentEntry> ComponentMap;

	TSharedPtr<SConstraintCanvas> Canvas;
};