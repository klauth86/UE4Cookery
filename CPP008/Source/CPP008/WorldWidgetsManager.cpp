#include "WorldWidgetsManager.h"
#include "SWorldWidgetsLayer.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SGameLayerManager.h"
#include "Kismet/GameplayStatics.h"

class FWorldWidgetsLayer : public IGameLayer {

public:

	FWorldWidgetsLayer(APlayerController* playerController) { PlayerController = playerController; }

	void AddComponent(UObject* owningObject, TSharedRef<SWidget> Widget, const FIntPoint& drawSize, const FVector2D& pivot, const bool& drawAtDesiredSize, const FVector& worldLocation, AActor* actor) {
		if (Components.Contains(owningObject)) return;
		
		Components.Emplace(owningObject);
		auto& Entry = Components[owningObject];

		Entry.OwningObject = owningObject;
		Entry.Widget = Widget;
		Entry.DrawSize = drawSize;
		Entry.Pivot = pivot;
		Entry.bDrawAtDesiredSize = drawAtDesiredSize;
		Entry.WorldLocation = worldLocation;
		Entry.Actor = actor;

		if (TSharedPtr<SWorldWidgetsLayer> ScreenLayer = ScreenLayerPtr.Pin()) ScreenLayer->AddComponent(owningObject, Entry);

		UE_LOG(LogTemp, Warning, TEXT("!!! AddComponent owningObject[%d] FWorldWidgetsLayer[%d] ScreenLayerPtr[%d]"), owningObject, this, ScreenLayerPtr.Pin().Get());
	}

	void RemoveComponent(UObject* owningObject) {
		if (!Components.Contains(owningObject)) return;

		Components.Remove(owningObject);

		if (TSharedPtr<SWorldWidgetsLayer> ScreenLayer = ScreenLayerPtr.Pin()) ScreenLayer->RemoveComponent(owningObject);

		UE_LOG(LogTemp, Warning, TEXT("!!! RemComponent owningObject[%d] FWorldWidgetsLayer[%d] ScreenLayerPtr[%d]"), owningObject, this, ScreenLayerPtr.Pin().Get());
	}

	bool HasComponent(UObject* owningObject) { return Components.Contains(owningObject); }

	virtual TSharedRef<SWidget> AsWidget() override {
		
		if (TSharedPtr<SWorldWidgetsLayer> ScreenLayer = ScreenLayerPtr.Pin()) return ScreenLayer.ToSharedRef();

		TSharedRef<SWorldWidgetsLayer> NewScreenLayer = SNew(SWorldWidgetsLayer, PlayerController.Get());
		ScreenLayerPtr = NewScreenLayer;

		for (auto componentItem : Components) {

			if (auto component = componentItem.Key.Get()) {
				auto& Entry = componentItem.Value;
				
				if (TSharedPtr<SWorldWidgetsLayer> ScreenLayer = ScreenLayerPtr.Pin()) ScreenLayer->AddComponent(component, Entry);
			}
		}

		return NewScreenLayer;
	}

private:
	TWeakObjectPtr<APlayerController> PlayerController;
	TWeakPtr<SWorldWidgetsLayer> ScreenLayerPtr;
	TMap<TWeakObjectPtr<UObject>, FComponentEntry> Components;
};

void WorldWidgetsManager::AddWorldWidget(UWorld* world, int32 playerControllerIndex, const FName& layerName, const int32 layerZOrder, UObject* owningObject, TSharedPtr<SWidget> widget, const FIntPoint& drawSize, const FVector2D& pivot, bool bDrawAtDesiredSize, const FVector& worldLocation, AActor* actor) {
	
	if (!owningObject) return;

	if (!widget.IsValid()) return;

	if (!world || !world->IsGameWorld()) return;

	auto playerController = UGameplayStatics::GetPlayerController(world, playerControllerIndex);
	auto localPlayer = playerController ? playerController->GetLocalPlayer() : nullptr;

	if (!playerController || !localPlayer) return;

	if (auto ViewportClient = world->GetGameViewport()) {
		auto LayerManager = ViewportClient->GetGameLayerManager();
		if (LayerManager.IsValid()) {
			
			TSharedPtr<FWorldWidgetsLayer> ScreenLayer;
			auto Layer = LayerManager->FindLayerForPlayer(localPlayer, layerName);
			
			if (!Layer.IsValid()) {
				TSharedRef<FWorldWidgetsLayer> NewScreenLayer = MakeShareable(new FWorldWidgetsLayer(playerController));
				LayerManager->AddLayerForPlayer(localPlayer, layerName, NewScreenLayer, layerZOrder);
				ScreenLayer = NewScreenLayer;
			}
			else {
				ScreenLayer = StaticCastSharedPtr<FWorldWidgetsLayer>(Layer);
			}

			ScreenLayer->AddComponent(owningObject, widget.ToSharedRef(), drawSize, pivot, bDrawAtDesiredSize, worldLocation, actor);
		}
	}
}

void WorldWidgetsManager::RemoveWorldWidget(UWorld* world, int32 playerControllerIndex, FName& layerName, UObject* owningObject) {

	if (!owningObject) return;

	if (!world || !world->IsGameWorld()) return;

	auto playerController = UGameplayStatics::GetPlayerController(world, playerControllerIndex);
	auto localPlayer = playerController ? playerController->GetLocalPlayer() : nullptr;

	if (!playerController || !localPlayer) return;

	if (auto ViewportClient = world->GetGameViewport()) {
		TSharedPtr<IGameLayerManager> LayerManager = ViewportClient->GetGameLayerManager();
		if (LayerManager.IsValid()) {
			
			TSharedPtr<IGameLayer> Layer = LayerManager->FindLayerForPlayer(localPlayer, layerName);
			if (Layer.IsValid()) {
				TSharedPtr<FWorldWidgetsLayer> ScreenLayer = StaticCastSharedPtr<FWorldWidgetsLayer>(Layer);
				ScreenLayer->RemoveComponent(owningObject);
			}
		}
	}
}

bool WorldWidgetsManager::HasWorldWidget(UWorld* world, int32 playerControllerIndex, FName& layerName, UObject* owningObject) {
	if (!owningObject) return false;

	if (!world || !world->IsGameWorld()) return false;

	auto playerController = UGameplayStatics::GetPlayerController(world, playerControllerIndex);
	auto localPlayer = playerController ? playerController->GetLocalPlayer() : nullptr;

	if (!playerController || !localPlayer) return false;

	if (auto ViewportClient = world->GetGameViewport()) {
		TSharedPtr<IGameLayerManager> LayerManager = ViewportClient->GetGameLayerManager();
		if (LayerManager.IsValid()) {

			TSharedPtr<IGameLayer> Layer = LayerManager->FindLayerForPlayer(localPlayer, layerName);
			if (Layer.IsValid()) {
				TSharedPtr<FWorldWidgetsLayer> ScreenLayer = StaticCastSharedPtr<FWorldWidgetsLayer>(Layer);
				return ScreenLayer->HasComponent(owningObject);
			}
		}
	}

	return false;
}

void WorldWidgetsManager::Reset(UWorld* world, int32 playerControllerIndex) {

	if (!world || !world->IsGameWorld()) return;

	auto playerController = UGameplayStatics::GetPlayerController(world, playerControllerIndex);
	auto localPlayer = playerController ? playerController->GetLocalPlayer() : nullptr;

	if (!playerController || !localPlayer) return;

	if (auto ViewportClient = world->GetGameViewport()) {
		
		auto LayerManager = ViewportClient->GetGameLayerManager();
		if (LayerManager.IsValid()) LayerManager->ClearWidgets();
	}
}