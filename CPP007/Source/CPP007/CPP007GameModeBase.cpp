// Copyright Epic Games, Inc. All Rights Reserved.


#include "CPP007GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SGameLayerManager.h"

void ACPP007GameModeBase::BeginPlay() {
	Super::BeginPlay();

	auto world = GetWorld();

	if (AddToViewportWidgetClass) {
		if (auto AddToViewportWidget = CreateWidget<UUserWidget>(world, AddToViewportWidgetClass)) {
			AddToViewportWidget->AddToViewport();
		}
	}

	auto controller = UGameplayStatics::GetPlayerController(this, 0);
	if (AddToPlayerScreenWidgetClass && controller) {
		if (auto AddToPlayerScreenWidget = CreateWidget<UUserWidget>(controller, AddToPlayerScreenWidgetClass)) {
			AddToPlayerScreenWidget->AddToPlayerScreen();
		}
	}

	if (LayerManagerWidgetClass && controller) {
		if (auto localPlayer = controller->GetLocalPlayer()) {
			if (auto ViewportClient = world->GetGameViewport()) {
				if (auto manager = ViewportClient->GetGameLayerManager()) {
					
					LayerManagerWidget = CreateWidget<UUserWidget>(controller, LayerManagerWidgetClass);
					if (LayerManagerWidget) {						
						manager->AddWidgetForPlayer(localPlayer, LayerManagerWidget->TakeWidget(), 0);
					}
				}
			}
		}
	}
}