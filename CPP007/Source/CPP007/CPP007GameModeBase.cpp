// Copyright Epic Games, Inc. All Rights Reserved.


#include "CPP007GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SGameLayerManager.h"

void ACPP007GameModeBase::BeginPlay() {
	Super::BeginPlay();

	auto world = GetWorld();

	if (AddToViewportWidgetClass_n10) {
		if (auto AddToViewportWidget = CreateWidget<UUserWidget>(world, AddToViewportWidgetClass_n10)) {
			AddToViewportWidget->AddToViewport(-10);
		}
	}

	if (AddToViewportWidgetClass_0) {
		if (auto AddToViewportWidget = CreateWidget<UUserWidget>(world, AddToViewportWidgetClass_0)) {
			AddToViewportWidget->AddToViewport();
		}
	}

	if (AddToViewportWidgetClass_p10) {
		if (auto AddToViewportWidget = CreateWidget<UUserWidget>(world, AddToViewportWidgetClass_p10)) {
			AddToViewportWidget->AddToViewport(10);
		}
	}

	auto controller = UGameplayStatics::GetPlayerController(this, 0);
	if (AddToPlayerScreenWidgetClass_n10 && controller) {
		if (auto AddToPlayerScreenWidget = CreateWidget<UUserWidget>(controller, AddToPlayerScreenWidgetClass_n10)) {
			AddToPlayerScreenWidget->AddToPlayerScreen(-10);
		}
	}

	if (AddToPlayerScreenWidgetClass_0 && controller) {
		if (auto AddToPlayerScreenWidget = CreateWidget<UUserWidget>(controller, AddToPlayerScreenWidgetClass_0)) {
			AddToPlayerScreenWidget->AddToPlayerScreen();
		}
	}

	if (AddToPlayerScreenWidgetClass_p10 && controller) {
		if (auto AddToPlayerScreenWidget = CreateWidget<UUserWidget>(controller, AddToPlayerScreenWidgetClass_p10)) {
			AddToPlayerScreenWidget->AddToPlayerScreen(10);
		}
	}

	auto localPlayer = controller->GetLocalPlayer();
	auto ViewportClient = world->GetGameViewport();

	if (localPlayer && ViewportClient) {
		if (auto manager = ViewportClient->GetGameLayerManager()) {

			if (LayerManagerWidgetClass_n10 && controller) {
				LayerManagerWidget_n10 = CreateWidget<UUserWidget>(controller, LayerManagerWidgetClass_n10);
				if (LayerManagerWidget_n10) {
					manager->AddWidgetForPlayer(localPlayer, LayerManagerWidget_n10->TakeWidget(), 0);
				}
			}

			if (LayerManagerWidgetClass_0 && controller) {
				LayerManagerWidget_0 = CreateWidget<UUserWidget>(controller, LayerManagerWidgetClass_0);
				if (LayerManagerWidget_0) {
					manager->AddWidgetForPlayer(localPlayer, LayerManagerWidget_0->TakeWidget(), 0);
				}
			}

			if (LayerManagerWidgetClass_p10 && controller) {
				LayerManagerWidget_p10 = CreateWidget<UUserWidget>(controller, LayerManagerWidgetClass_p10);
				if (LayerManagerWidget_p10) {
					manager->AddWidgetForPlayer(localPlayer, LayerManagerWidget_p10->TakeWidget(), 0);
				}
			}
		}
	}
}