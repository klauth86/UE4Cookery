// Fill out your copyright notice in the Description page of Project Settings.

#include "SWorldWidgetsLayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Slate/SGameLayerManager.h"
#include "SceneView.h"

void SWorldWidgetsLayer::Construct(const FArguments& InArgs, APlayerController* playerController) {
	
	PlayerControllerPtr = playerController;

	bCanSupportFocus = false;

	ChildSlot[SAssignNew(Canvas, SConstraintCanvas)];
}

void SWorldWidgetsLayer::AddComponent(UObject* owningObject, const FComponentEntry& entry) {
	if (owningObject) {
		FComponentEntry& Entry = ComponentMap.FindOrAdd(FObjectKey(owningObject));
		Entry.OwningObject = entry.OwningObject;
		Entry.Widget = entry.Widget;
		Entry.DrawSize = entry.DrawSize;
		Entry.Pivot = entry.Pivot;
		Entry.bDrawAtDesiredSize = entry.bDrawAtDesiredSize;
		Entry.WorldLocation = entry.WorldLocation;
		Entry.Actor = entry.Actor;
		Canvas->AddSlot().Expose(Entry.Slot)[SAssignNew(Entry.ContainerWidget, SBox)[Entry.Widget.ToSharedRef()]];
	}
}

void SWorldWidgetsLayer::RemoveComponent(UObject* owningObject) {
	if (ensure(owningObject)) {
		if (FComponentEntry* EntryPtr = ComponentMap.Find(owningObject)) {
			if (!EntryPtr->bRemoving) {
				RemoveEntryFromCanvas(*EntryPtr);
				ComponentMap.Remove(owningObject);
			}
		}
	}
}

void SWorldWidgetsLayer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	if (auto PlayerController = PlayerControllerPtr.Get()) {
		
		if (UGameViewportClient* ViewportClient = PlayerController->GetWorld()->GetGameViewport()) {
			const FGeometry& ViewportGeometry = ViewportClient->GetGameLayerManager()->GetViewportWidgetHostGeometry();

			// cache projection data here and avoid calls to UWidgetLayoutLibrary.ProjectWorldLocationToWidgetPositionWithDistance
			FSceneViewProjectionData ProjectionData;
			FMatrix ViewProjectionMatrix;
			bool bHasProjectionData = false;

			ULocalPlayer const* const LP = PlayerController->GetLocalPlayer();
			if (LP && LP->ViewportClient) {
				bHasProjectionData = LP->GetProjectionData(ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData);
				if (bHasProjectionData) {
					ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
				}
			}

			for (auto It = ComponentMap.CreateIterator(); It; ++It) {
				FComponentEntry& Entry = It.Value();

				if (auto owningObject = Entry.OwningObject.Get()) {

					auto worldLocation = Entry.Actor.IsValid() ? Entry.Actor->GetActorLocation() + Entry.WorldLocation : Entry.WorldLocation;

					FVector2D ScreenPosition2D;
					const bool bProjected = bHasProjectionData ? FSceneView::ProjectWorldToScreen(worldLocation, ProjectionData.GetConstrainedViewRect(), ViewProjectionMatrix, ScreenPosition2D) : false;
					if (bProjected) {
						const float ViewportDist = FVector::Dist(ProjectionData.ViewOrigin, worldLocation);
						const FVector2D RoundedPosition2D(FMath::RoundToInt(ScreenPosition2D.X), FMath::RoundToInt(ScreenPosition2D.Y));
						FVector2D ViewportPosition2D;
						USlateBlueprintLibrary::ScreenToViewport(PlayerController, RoundedPosition2D, ViewportPosition2D);

						const FVector ViewportPosition(ViewportPosition2D.X, ViewportPosition2D.Y, ViewportDist);

						Entry.ContainerWidget->SetVisibility(EVisibility::SelfHitTestInvisible);

						if (SConstraintCanvas::FSlot* CanvasSlot = Entry.Slot) {
							FVector2D AbsoluteProjectedLocation = ViewportGeometry.LocalToAbsolute(FVector2D(ViewportPosition.X, ViewportPosition.Y));
							FVector2D LocalPosition = AllottedGeometry.AbsoluteToLocal(AbsoluteProjectedLocation);

							FIntPoint& drawSize = Entry.DrawSize;

							CanvasSlot->AutoSize(drawSize.SizeSquared() == 0 || Entry.bDrawAtDesiredSize);
							CanvasSlot->Offset(FMargin(LocalPosition.X, LocalPosition.Y, drawSize.X, drawSize.Y));
							CanvasSlot->Anchors(FAnchors(0, 0, 0, 0));
							CanvasSlot->Alignment(Entry.Pivot);

							CanvasSlot->ZOrder(-ViewportPosition.Z);
						}
					}
					else {
						Entry.ContainerWidget->SetVisibility(EVisibility::Collapsed);
					}
				}
				else {
					RemoveEntryFromCanvas(Entry);
					It.RemoveCurrent();
					continue;
				}
			}

			// Done
			return;
		}
	}

	if (GSlateIsOnFastUpdatePath) {
		// Hide everything if we are unable to do any of the work.
		for (auto It = ComponentMap.CreateIterator(); It; ++It) {
			FComponentEntry& Entry = It.Value();
			Entry.ContainerWidget->SetVisibility(EVisibility::Collapsed);
		}
	}
}

void SWorldWidgetsLayer::RemoveEntryFromCanvas(FComponentEntry& Entry) {
	Entry.bRemoving = true;

	if (TSharedPtr<SWidget> ContainerWidget = Entry.ContainerWidget) {
		Canvas->RemoveSlot(ContainerWidget.ToSharedRef());
	}
}